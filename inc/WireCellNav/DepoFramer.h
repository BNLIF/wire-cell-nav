#ifndef WIRECELL_DEPOFRAMER
#define WIRECELL_DEPOFRAMER

#include "WireCellIface/IWire.h"
#include "WireCellIface/IDepo.h"
#include "WireCellIface/IFrame.h"

#include "WireCellUtil/Units.h"

namespace WireCell {

    


    /** Take some depositions and produce corresponding frames.
     *
     * Depositions are assumed to be in time order.
     */
    class DepoFramer : public IWireSink, public IDepoSink, public IFrameSequence {
    public:

	/** Create a DepoFramer.
	 *
	 * \param frame_time is the amount of time the resulting frame readout spans.
	 *
	 * \param frame_drift is the distance along the X-axis (drift
	 * axis) which is active.
	 */
	DepoFramer(double frame_time = 1*units::ms,
		   double frame_drift = 1*units::meter);
	virtual ~DepoFramer();

	/// Configure the object.
	// void configure(FrameConfigObject....);
	// fixme: make configurable.

	/// Give input wires.
	void sink(wire_iterator begin, wire_iterator end) {
	    m_wires = wire_range(begin,end);
	}

	/// Give input depositions.
	void sink(depo_iterator begin, depo_iterator end) {
	    m_depos = depo_range(begin, end);
	}

	/// Access the frames that this DepoFramer makes.
	WireCell::frame_iterator frames_begin() const;
	WireCell::frame_iterator frames_end() const;
    private:
	double m_start_time;
	wire_range m_wires;
	depo_range m_depos;
    };
    
}

#endif
