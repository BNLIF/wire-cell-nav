#ifndef WIRECELL_DEPOTRANSPORT
#define WIRECELL_DEPOTRANSPORT

#include "WireCellIface/IDepo.h"

#include "WireCellUtil/Units.h"

#include <string>

namespace WireCell {


    /** Transport (drift) a sequence of depositions in a volume to a
     * plane at a given location on the X-axis.
     */

    class DepoTransport {
    public:
	DepoTransport(depoptr_iterator begin, depoptr_iterator end,
		      double location=0*units::meter,
		      double drift_velocity = 1.6*units::mm/units::microsecond);

	/// The next IDepo, transported to the location.
	IDepoPtr operator()();

	/// Return false once there are no more IDepo objects.
	operator bool() const;

	/// Compare equality against another DepoTransport.
	bool operator==(const DepoTransport& rhs) const;

	std::string dump(const IDepoPtr& p);
	IDepoPtr top();
	IDepoPtr bot();
	IDepoPtr pop();

    private:
	typedef std::set<IDepoPtr, IDepoPtrDriftCompare> DepoPtrVector;
	DepoPtrVector m_buffer;
	double m_drift_velocity;
	depoptr_iterator m_it, m_end;

	// read recent input stream
	bool buffer();	
    };

    /// Return a transported range of IDepoPtrs. 
    depoptr_range depo_transport(depoptr_iterator begin, depoptr_iterator end,
				 double location=0*units::meter,
				 double drift_velocity = 1.6*units::mm/units::microsecond);

}

#endif
