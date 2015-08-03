#ifndef WIRECELL_DIGITIZER
#define WIRECELL_DIGITIZER

#include "WireCellIface/IWire.h"
#include "WireCellUtil/Units.h"

namespace WireCell {

    class Digitizer, public IWireSink {
	typedef boost::signals2::signal<IDepo::const_ptr ()> DepoFeed;
    public:
	/// A slot which feeds the drifter depositions.  Each time it
	/// is called it should return the next available deposition
	/// in strict time order.
	typedef DepoFeed::slot_type DepoFeeder;

	/// Make a simple digitizer for the given wire plane and
	/// digitization tick.
	Digitizer(double tick = 0.5*units::microsecond);
	// fixme: configurable

	/// Set the wires to use (IWireSink).  
	virtual void sink(wire_iterator begin, wire_iterator end);

	/// Connect a feed of depositions at the wire plane
	void connect(const DepoFeeder& feed);


	/// Produce a frame.
	IFrame::const_ptr operator()();

    };

}

#endif
