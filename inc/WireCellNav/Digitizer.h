#ifndef WIRECELL_DIGITIZER
#define WIRECELL_DIGITIZER

#include "WireCellIface/IWireSummary.h"
#include "WireCellIface/IDepo.h"
#include "WireCellIface/IFrame.h"
#include "WireCellUtil/Units.h"


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
    class Digitizer : public IWireSummaryClient {
	typedef boost::signals2::signal<IDepo::pointer ()> DepoFeed;
	DepoFeed m_feed;
	const int m_maxticks;
	const double m_tick;
	double m_time;
	IDepo::pointer m_depo;
	int m_frame_count;

    public:
	/// A slot which feeds the drifter depositions.  Each time it
	/// is called it should return the next available deposition
	/// in strict time order.
	typedef DepoFeed::slot_type DepoFeeder;

	/// Make a simple digitizer for the given wire plane and
	/// digitization tick.  The traces in the resulting frames
	/// will not span more than maxticks.
	Digitizer(int maxticks = 1000,
		  double tick = 0.5*units::microsecond,
		  double start_time = 0*units::second);
	// fixme: configurable

	/// Connect a feed of depositions at the wire plane.  This
	/// digitizer assumes depositions are provided already drifted
	/// to the wire plane.
	void connect(const DepoFeeder& feed);

	/// Produce a frame.
	IFrame::pointer operator()();

    };

}

#endif
