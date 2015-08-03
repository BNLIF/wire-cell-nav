#include "WireCellNav/Drifter.h"
#include "TransportedDepo.h"

#include <boost/range.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace WireCell;


Drifter::Drifter(double location,
		 double drift_velocity)
    : m_location(location)
    , m_drift_velocity(drift_velocity)
    , m_buffer(IDepoPtrDriftCompare(drift_velocity))
{
}

void Drifter::connect(const DepoFeeder& feed)
{
    m_feed.connect(feed);
}

double Drifter::proper_time(IDepo::const_ptr depo) {
    return depo->time() + depo->pos().x()/m_drift_velocity;
}

bool Drifter::buffer()
{
    if (!m_buffer.size()) {	// empty buffer, prime.
	IDepo::const_ptr depo = *m_feed();
	if (!depo) {
	    return false;	// no buffer, no input, done.
	}
	m_buffer.insert(depo);
    }

    while (true) {
	double tau = proper_time(top());
	IDepo::const_ptr latest = bot();
	if (latest->time() >= tau) {
	    return true;
	}
	latest = *m_feed();
	if (!latest) {
	    break;
	}
	m_buffer.insert(latest);
    }

    return m_buffer.size() > 0;
}

// return value and increment
IDepo::const_ptr Drifter::operator()()
{
    if ( !buffer() ) {
	return 0;
    }
    IDepo::const_ptr before = pop();
    IDepo::const_ptr ret(new TransportedDepo(before, m_location, m_drift_velocity));
    return ret;
}

IDepo::const_ptr Drifter::pop()
{
    IDepo::const_ptr ret = top();
    m_buffer.erase(ret);
    return ret;
}

IDepo::const_ptr Drifter::top()
{
    return *m_buffer.begin();
}
IDepo::const_ptr Drifter::bot()
{
    return *m_buffer.rbegin();
}


