#ifndef WIRECELL_DEPOTRANSPORTER
#define WIRECELL_DEPOTRANSPORTER

#include "WireCellIface/IDepo.h"

#include "WireCellUtil/Units.h"

#include <string>

namespace WireCell {


    /** Transporter (drifter).  
     *
     * This reads depositions from a IDepoSource and produces new
     * depositions transported to a location on the X-axis.  It takes
     * care of properly ordering the depositions in time as well as
     * adjusting their time and X value to be what they should be
     * given the final location and a drift velocity.
     *
     * The resulting depos have a pointer back to their original depo.
     *
     * The original sequence of depos may be "infinite" and the
     * transported ones will be as well.  There is some buffering and
     * reordering to the extent needed to assure all recent depos are
     * checked in order to collapse time/X-distance ambiguity.
     *
     * The input IDepoSource must produce depositions in time order.
     */

    class DepoTransporter : public IDepoSource {
    public:
	DepoTransporter(IDepoSource& src,
			double location=0*units::meter,
			double drift_velocity = 1.6*units::mm/units::microsecond);

	/// The next IDepo, transported to the location.
	virtual IDepo::const_ptr depo_gen();

	IDepoPtr top();
	IDepoPtr bot();
	IDepoPtr pop();

    private:
	typedef std::set<IDepo::const_ptr, IDepoPtrDriftCompare> DepoBuffer;
	IDepoSource &m_src;
	DepoBuffer m_buffer;
	double m_location;
	double m_drift_velocity;

	// read recent input stream
	bool buffer();	
    };

}

#endif
