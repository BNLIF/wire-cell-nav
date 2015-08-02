#ifndef WIRECELL_DEPOCOLLECTION
#define WIRECELL_DEPOCOLLECTION

#include "WireCellIface/IDepo.h"

namespace WireCell {

    /** A source of ordered depositions fed from a sequence.
     */
    class DepoCollection : public IDepoSource {
	depoptr_range m_depos;
    public:
	DepoCollection(depoptr_range depos);
	virtual ~DepoCollection();

	virtual IDepo::const_ptr depo_gen();
    };
}

#endif

