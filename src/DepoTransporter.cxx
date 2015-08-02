#include "WireCellNav/DepoTransporter.h"
#include "TransportedDepo.h"

#include <boost/range.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace WireCell;


DepoTransporter::DepoTransporter(IDepoSource& src,
				 double location, double drift_velocity)
    : m_src(src)
    , m_buffer(IDepoPtrDriftCompare(drift_velocity))
    , m_location(location)
    , m_drift_velocity(drift_velocity)
{
}

bool DepoTransporter::buffer()
{
    double tau=0.0;
    IDepo::const_ptr next;
    if (!m_buffer.size()) {
	next = m_src.depo_gen();
	if (!next) {
	    return false;
	}
	m_buffer.insert(next);
	tau = next->time() + next->pos().x()/m_drift_velocity;
    }	
    else {
	tau = top()->time() + top()->pos().x()/m_drift_velocity;
    }

    if (tau > bot()->time()) {
	return true;
    }

    // read until we pass tau
    while ((next = m_src.depo_gen())) {
	if (!next) {
	    break;
	}
	m_buffer.insert(next);
	if (next->time() > tau) {
	    return true;
	}
    }

    return m_buffer.size() > 0;
}

// return value and increment
IDepo::const_ptr DepoTransporter::depo_gen()
{
    if ( !buffer() ) {
	return 0;
    }
    IDepoPtr before = pop();
    IDepoPtr ret(new TransportedDepo(before, m_location, m_drift_velocity));
    return ret;
}

IDepoPtr DepoTransporter::pop()
{
    IDepoPtr ret = top();
    m_buffer.erase(ret);
    return ret;
}

IDepoPtr DepoTransporter::top()
{
    return *m_buffer.begin();
}
IDepoPtr DepoTransporter::bot()
{
    return *m_buffer.rbegin();
}


