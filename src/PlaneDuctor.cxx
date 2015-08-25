#include "WireCellNav/PlaneDuctor.h"
#include "WireCellNav/Diffuser.h"

#include "WireCellUtil/BufferedHistogram2D.h"

#include <iostream>
using namespace std;		// debugging
using namespace WireCell;


// fixme: this big constructor is kind of a mess.  In any case, this
// class needs to implement a "plane ductor" interface and be an
// configurable.
PlaneDuctor::PlaneDuctor(const Ray& pitch,
			 double tick,
			 double tstart,
			 double drift_velocity,
			 double tbuffer,
			 double DL,
			 double DT,
			 int nsigma)
    : m_pitch_origin(pitch.first)
    , m_pitch_direction(ray_unit(pitch))
    , m_drift_velocity(drift_velocity)
    , m_tbuffer(tbuffer)
    , m_DL(DL)
    , m_DT(DT)
    , m_hist(new BufferedHistogram2D(tick, ray_length(pitch), tstart, 0))
    , m_diff(new Diffuser(*m_hist, nsigma))
    ,  m_high_water_tau(tstart)

{
    // The histogram's X direction is in units of "proper time" (tau)
    // which is defined as the absolute time it is when a point event
    // drifts from where/when it occurred to the plane.  To handle
    // diffusion, the point deposition is placed at a proper time in
    // the future by the amount of buffering requested.

    // cerr << "PlaneDuctor: BufferedHistogram2D: "
    // 	 << "xmin=" << m_hist->xmin() << " xbinsize=" << m_hist->xbinsize() << ", "
    // 	 << "ymin=" << m_hist->ymin() << " ybinsize=" << m_hist->ybinsize() << endl;

}
PlaneDuctor::~PlaneDuctor()
{
    delete m_diff;
    delete m_hist;
}

double PlaneDuctor::proper_tau(double event_time, double event_xloc)
{
    return event_time + (event_xloc - m_pitch_origin.x()) / m_drift_velocity;
}
double PlaneDuctor::pitch_dist(const Point& p)
{
    Vector to_p = p - m_pitch_origin;
    return m_pitch_direction.dot(to_p);
}


void PlaneDuctor::buffer()
{
    int count = 0;
    double charge_buffered = 0;

    // consume depositions until tbuffer amount of time past "now".
    while (m_high_water_tau < clock() + m_tbuffer) {
	IDepo::pointer depo = fire();
	if (!depo) {
	    return;
	}

	++count;
	charge_buffered += depo->charge();

	// the time it will show up
	double tau = proper_tau(depo->time(), depo->pos().x());

	// how long the center will have drifted for calculating diffusion sigmas.
	IDepo::pointer initial_depo = *depo_chain(depo).rbegin();
	double drift_time = proper_tau(0, initial_depo->pos().x());
	double tmpcm2 = 2*m_DL*drift_time/units::centimeter2;
	double sigmaL = sqrt(tmpcm2)*units::centimeter / m_drift_velocity;
	double sigmaT = sqrt(2*m_DT*drift_time/units::centimeter2)*units::centimeter2;
	double trans = pitch_dist(depo->pos());
	m_diff->diffuse(tau, trans, sigmaL, sigmaT, depo->charge());
	// cerr << "Diffuse tau="<<tau << " +/- "<<sigmaL<<" trans=" << trans << " +/- " << sigmaT << endl;

	if (tau > m_high_water_tau) {
	    m_high_water_tau = tau;
	}
    }
    // if (count) {
    // 	cerr << "PlaneDuctor: buffered q=" << charge_buffered
    // 	     << " in " << count << " deposits at t=" << clock()
    // 	     << " up to tau=" << m_high_water_tau << endl;
    // }

}


double PlaneDuctor::clock()
{
    return m_hist->xmin();
}

std::vector<double> PlaneDuctor::latch()
{
    buffer();
    return m_hist->popx();    
}

int PlaneDuctor::buffer_size()
{
    buffer();
    return m_hist->size();
}
