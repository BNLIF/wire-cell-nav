#ifndef WIRECELL_DEPOTRANSPORT
#define WIRECELL_DEPOTRANSPORT

#include "WireCellIface/IDepo.h"

#include "WireCellUtil/Units.h"

namespace WireCell {


    /** Transport (drift) a sequence of depositions in a volume to a
     * plane at a given location on the X-axis.
     */

    class DepoTransport {
    public:
	DepoTransport(depo_iterator begin, depo_iterator end, double x=0);
	
	

    };

    
........................

use GeneratorIter

................................g



    class DepoTransport : public IDepoSink, public IDepoSequence {
    public:

	/** Create a DepoTransport.
	 *
	 * \param location gives the value along the X-axis of the
	 * plane to which the depositions are transported.
	 */
	DepoTransport(double location = 0*units::meter);
	virtual ~DepoTransport();

	/// Configure the object.
	// void configure(FrameConfigObject....);
	// fixme: make configurable.

	/// Give input depositions.
	void sink(depo_iterator begin, depo_iterator end) {
	    m_depos = depo_range(begin, end);
	}

	/// Access the frames that this DepoTransport makes.
	depo_iterator depos_begin() const;
	depo_iterator depos_end() const;

    private:

	double m_start_time;

    };
    
}

#endif
