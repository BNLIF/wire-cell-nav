#ifndef WIRECELL_DIGITIZER
#define WIRECELL_DIGITIZER

#include "WireCellIface/IWireSummary.h"
#include "WireCellIface/IDepo.h"
#include "WireCellIface/IFrame.h"
#include "WireCellUtil/Units.h"
#include "WireCellUtil/Signal.h"


namespace WireCell {

    /** A simple digitizer of charge drifting past wire planes.
     *
     * Each call produces an IFrame which spans a given number of
     * ticks each of a given size.
     *
     * This digitizer is simple in that it:
     *
     * - the planes are taken to be coplanar
     * - the depositions are assumed to be delivered at the common plane 
     * - the drifting charge is directly turned into signal (no response function, no deconvolution)
     * - only the closest wire is "hit" (even for induction planes)
     */
    class Digitizer : public Signal<IDepo>, public IWireSummaryClient {
	const int m_maxticks;
	const double m_tick;
	double m_time;
	IDepo::pointer m_depo;
	int m_frame_count;

    public:

	/// Make a simple digitizer for the given wire plane and
	/// digitization tick.  The traces in the resulting frames
	/// will not span more than maxticks.
	Digitizer(int maxticks = 1000,
		  double tick = 0.5*units::microsecond,
		  double start_time = 0*units::second);
	// fixme: configurable

	/// Produce a frame.
	IFrame::pointer operator()();

    };

}

#endif
