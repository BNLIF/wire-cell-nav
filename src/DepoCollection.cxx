#include "WireCellNav/DepoCollection.h"

using namespace WireCell;

DepoCollection::DepoCollection(depoptr_range depos)
    : m_depos(depos)
{
}

DepoCollection::~DepoCollection()
{
    
}

IDepo::const_ptr DepoCollection::depo_gen()
{
    if (m_depos.first == m_depos.second) {
	return nullptr;
    }

    IDepo::const_ptr ret = *m_depos.first;
    ++m_depos.first;
    return ret;
}
