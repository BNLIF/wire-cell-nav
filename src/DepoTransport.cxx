#include "WireCellNav/DepoTransport.h"
#include "WireCellUtil/GeneratorIter.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace WireCell;

DepoTransport::DepoTransport(depoptr_iterator begin, depoptr_iterator end,
			     double location, double drift_velocity)
    : m_buffer(IDepoPtrDriftCompare(drift_velocity))
    , m_drift_velocity(drift_velocity)
    , m_it(begin)
    , m_end(end)
{
    // cerr << "DepoTransport to " << location << " at speed " << drift_velocity << endl;
}

	
bool DepoTransport::buffer()
{
    // if no upstream, no point in buffering
    if (m_it == m_end) {
	// cerr << "buffer: end of transport" << endl;
	return m_buffer.size() > 0;
    }

    // empty, prime the pump
    if (!m_buffer.size()) {
	IDepoPtr prime(*m_it);
	m_buffer.insert(prime);
	++m_it;
    }

    // get minimum "proper time".
    IDepoPtr closest = top();
    double tau = closest->time() + closest->pos().x()/m_drift_velocity;

    // advance buffer to include any and all local times less than
    // minimum proper time.
    while (m_it != m_end) {
	IDepoPtr prime(*m_it);
	double next = prime->time();
	if (next > tau) {
	    cerr << "buffer: end of high water mark: " << this->dump(*m_it) << endl;
	    break;		// past our time, leave for later
	}
	m_buffer.insert(prime);
	++m_it;
    }
    cerr << "buffer: buffering " << m_buffer.size() << endl;
    return m_buffer.size() > 0;
}

std::string DepoTransport::dump(const IDepoPtr& p)
{
    stringstream ss;
    double x = p->pos().x(), t = p->time(), tau = x/m_drift_velocity;
    ss << "<IDepo tau:" << tau << " t:" << t << " x:" << x << " q:" << p->charge() << ">";
    return ss.str();
}

// return value and increment
IDepoPtr DepoTransport::operator()()
{
    if ( !buffer() ) {
	// cerr << (void*)this << " operator(): buffer no more buffer" << endl;
	return 0;
    }
    // cerr << (void*)this << " buffer size: "  << m_buffer.size() 
    // 	 << " top: " << this->dump(top())
    // 	 << " bot: " << this->dump(bot())
    // 	 << endl;

    return pop();
}

IDepoPtr DepoTransport::pop()
{
    IDepoPtr ret = top();
    m_buffer.erase(ret);
    return ret;
}

IDepoPtr DepoTransport::top()
{
    return *m_buffer.begin();
}
IDepoPtr DepoTransport::bot()
{
    return *m_buffer.rbegin();
}

DepoTransport::operator bool() const
{
    // if (m_buffer.size() == 0) {
    // 	cerr << "transport buffer is empty" << endl;	
    // }
    // if (m_it == m_end) {
    // 	cerr << "transport stream is dry" << endl;
    // }
    bool empty = ((m_buffer.size() == 0) && (m_it == m_end));
    // if (empty) {
    // 	cerr << "transport is exhausted"  << endl;
    // }
    return !empty;
}

bool DepoTransport::operator==(const DepoTransport& rhs) const
{
    if (this == &rhs) return true;
    if (m_it == rhs.m_it) return true; // is this the right thing to do?
    return false;
}

    
typedef GeneratorIter<DepoTransport, IDepoPtr> gen_depoptr_iter;

typedef IteratorAdapter< gen_depoptr_iter, WireCell::depoptr_base_iterator > gen_iter;
    

depoptr_range WireCell::depo_transport(depoptr_iterator begin, depoptr_iterator end,
				       double location, double drift_velocity)
{
    gen_depoptr_iter gen_begin(DepoTransport(begin, end, location, drift_velocity));
    gen_depoptr_iter gen_end(DepoTransport(end, end, location, drift_velocity));

    return depoptr_range(gen_iter(gen_begin), gen_iter(gen_end));
}

