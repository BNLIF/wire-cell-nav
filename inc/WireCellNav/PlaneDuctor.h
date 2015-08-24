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


	PlaneDuctor(const Ray& pitch, // location, pitch and angle of wires
		    double tick = 0.5*units::microsecond, // digitized time bin size
		    double tstart = 0.0*units::microsecond); // absolute time of first tick

	~PlaneDuctor();

	/// Return the time of the internal clock.
	double clock();
	
	/// Return a vector filled with per wire (segment) charge
	/// indexed by wire index.  Trailing wires with zero charge are
	/// suppressed.  This advances the internal clock.
	std::vector<double> latch();

    private:
	struct Config;
	Config* m_config;
	BufferedHistogram2D* m_hist;
	Diffuser* m_diff;

	double m_high_water;
	void buffer();
    };

}

#endif
