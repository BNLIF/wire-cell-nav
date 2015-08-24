#include "WireCellNav/PlaneDuctor.h"
#include "WireCellNav/Diffuser.h"

#include "WireCellUtil/BufferedHistogram2D.h"

using namespace WireCell;

// eventually make this a configurable?
struct PlaneDuctor::Config {
    const Ray pitch;
    const Vector pitch_unit;
    double tick, tstart, tbuffer;
    double drift_velocity;
    double DL, DT;
    int nsigma;

    /** Note: tbuffer must be bigger than maximum longitudinal N*sigma
     * for the detector. */

    Config(const Ray& pitch, // location, pitch and angle of wires
	   double tick = 0.5*units::microsecond, // digitized time bin
	   double tstart = 0.0*units::microsecond, // start the clock
	   double tbuffer = 5.0*units::microsecond, // how long into the past to buffer 
	   double drift_velocity = 1.6*units::millimeter/units::microsecond, 
	   double DL=5.3*units::centimeter2/units::second, // long. diffusion coefficient
	   double DT=12.8*units::centimeter2/units::second, // trans. diffusion coefficient
	   int nsigma=3) // number of sigma of diffusion to keep
	: pitch(pitch), pitch_unit(ray_unit(pitch)), tick(tick), tstart(tstart), tbuffer(tbuffer)
	, drift_velocity(drift_velocity)
	, DL(DL), DT(DT), nsigma(nsigma)
	{
	}

    double tau(double event_time, double event_xloc) {
	return event_time + (event_xloc - pitch.first.x()) / drift_velocity;
    }
    double pitch_dist(const Point& p) {
	Vector to_p = p - pitch.first;
	return pitch_unit.dot(to_p);
    }
};


// fixme: this constructor is a mess.
PlaneDuctor::PlaneDuctor(const Ray& pitch, double tick, double tstart)
    : m_config(new PlaneDuctor::Config(pitch, tick, tstart))
    , m_hist(new BufferedHistogram2D(tstart, ray_length(m_config->pitch), tick,-0.5*ray_length(m_config->pitch)))
    , m_diff(new Diffuser(*m_hist, m_config->nsigma))
    , m_high_water(tstart)
{
    // The histogram's X direction is in units of "proper time" (tau)
    // which is defined as the absolute time it is when a point event
    // drifts from where/when it occurred to the plane.  To handle
    // diffusion, the point deposition is placed at a proper time in
    // the future by the amount of buffering requested.

}
PlaneDuctor::~PlaneDuctor()
{
    delete m_diff;
    delete m_hist;
    delete m_config;
}

    void PlaneDuctor::buffer()
{
    // consume depositions until tbuffer amount of time past "now".
    while (m_high_water < clock() + m_config->tbuffer) {
	IDepo::pointer depo = fire();
	// the time it will show up
	double tau = m_config->tau(depo->time(), depo->pos().x());

	// how long the center will have drifted for calculating diffusion sigmas.
	IDepo::pointer initial_depo = *depo_chain(depo).rbegin();
	double drift_time = m_config->tau(0, initial_depo->pos().x());
	double tmpcm2 = 2*m_config->DL*drift_time/units::centimeter2;
	double sigmaL = sqrt(tmpcm2)*units::centimeter / m_config->drift_velocity;
	double sigmaT = sqrt(2*m_config->DT*drift_time/units::centimeter2)*units::centimeter2;

	m_diff->diffuse(tau, m_config->pitch_dist(depo->pos()), sigmaL, sigmaT, depo->charge());

	if (tau > m_high_water) {
	    m_high_water = tau;
	}
    }
}


double PlaneDuctor::clock()
{
    return m_hist->xmin();
}

std::vector<double> PlaneDuctor::latch()
{
    return m_hist->popx();    
}
