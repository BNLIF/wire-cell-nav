/**
   The PlaneDuctor (plane-inductor/plane-conductor) produces digitized
   signals on its plane of wires from depositions that have been
   drifted near by.
 */

#ifndef WIRECELL_PLANEDUCTOR
#define WIRECELL_PLANEDUCTOR

#include "WireCellUtil/Signal.h"
#include "WireCellIface/IDepo.h"

namespace WireCell {

    class BufferedHistogram2D;
    class Diffuser;

    class PlaneDuctor : public Signal<IDepo> { // fixme make into interface
    public:


	PlaneDuctor(const Ray& pitch, /// location, pitch and angle of wires
		    double tick = 0.5*units::microsecond, /// digitized time bin size
		    double tstart = 0.0*units::microsecond, /// absolute time of first tick
		    double drift_velocity = 1.6*units::millimeter/units::microsecond, 
		    double tbuffer = 5.0*units::microsecond, /// how long into the past to buffer 
		    double DL=5.3*units::centimeter2/units::second, /// long. diffusion coefficient
		    double DT=12.8*units::centimeter2/units::second, /// trans. diffusion coefficient
		    int nsigma=3); /// number of sigma of diffusion to keep
	


	~PlaneDuctor();

	/// Return the current buffer depth.  When zero, there is no more data.
	int buffer_size();

	/// Return the time of the internal clock.  Calling latch()
	/// will advance this internal clock.
	double clock();
	
	/// Return a vector filled with per wire (segment) charge
	/// indexed by wire index.  Trailing wires with zero charge are
	/// suppressed.  This advances the internal clock.
	std::vector<double> latch();

	// Internal method for calculating the "proper time" of an
	// event.  This is the time for a point like event occurring
	// at t,x to drift to the location of our wire plane (as
	// determined by the tail of <pitch>).
	double proper_tau(double event_time, double event_xloc);

	// Internal method for calculating the distance along the
	// pitch from the origin of the wires (as determined by the
	// <pitch> ray) that the projection of the 3D point makes on
	// the wire plane.
	double pitch_dist(const Point& p);

    private:
	const Vector m_pitch_origin;
	const Vector m_pitch_direction;
	const double m_drift_velocity;
	const double m_tbuffer;
	const double m_DL;
	const double m_DT;

	BufferedHistogram2D* m_hist;
	Diffuser* m_diff;
	double m_high_water_tau;

	// read-ahead enough depositions to surpass the <tbuffer> time from the current "now".
	void buffer();
    };

}

#endif
