#include "WireCellNav/DepoTransport.h"
#include "WireCellUtil/GeneratorIter.h"



class TransportedDepo : public WireCell::IDepo {
    WireCell::IDepoPtr m_from;
    WireCell::Point m_pos;
    double m_time;
public:

    TransportedDepo(const WireCell::IDepoPtr& from, double location, double velocity) 
	: m_from(from), m_pos(from->pos()) {
	double dx = m_pos.x() - location;
	m_pos.x(location);
	m_time = from->time() + dx/velocity;
    }
    virtual ~TransportedDepo() {};

    virtual const WireCell::Point& pos() const { return m_pos; }
    virtual double time() const { return m_time; }
    virtual double charge() const { return m_from->charge(); }
    virtual WireCell::IDepoPtr original() { return m_from; }
};


#include <iostream>
#include <sstream>

using namespace std;
using namespace WireCell;


DepoTransport::DepoTransport(depoptr_iterator begin, depoptr_iterator end,
			     double location, double drift_velocity)
    : m_buffer(IDepoPtrDriftCompare(drift_velocity))
    , m_location(location)
    , m_drift_velocity(drift_velocity)
    , m_it(begin)
    , m_end(end)
{
    // if (begin == end) {
    // 	cerr << "DepoTransport (end) @" << (void*)this << endl;
    // }
    // else {
    // 	cerr << "DepoTransport (beg) @ "<< (void*)this << " to " << location << " at speed " << drift_velocity << endl;
    // }
}

DepoTransport::DepoTransport(const DepoTransport& rhs)
    : m_buffer(rhs.m_buffer.begin(), rhs.m_buffer.end())
    , m_location(rhs.m_location)
    , m_drift_velocity(rhs.m_drift_velocity)
    , m_it(rhs.m_it)
    , m_end(rhs.m_end)
{
    // cerr << "DepoTransport (copy) @" << (void*)(&rhs) << "  -->  @" << (void*)this << endl;
}

DepoTransport& DepoTransport::operator=(const DepoTransport& rhs)
{
    // cerr << "DepoTransport (assign) @" << (void*)(&rhs) << "  -->  @" << (void*)this << endl;

    m_buffer.clear();
    m_buffer.insert(rhs.m_buffer.begin(), rhs.m_buffer.end());
    m_location = rhs.m_location;
    m_drift_velocity = rhs.m_drift_velocity;
    m_it = rhs.m_it;
    m_end = rhs.m_end;
    return *this;
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
	++m_it;
	double next = prime->time();
	if (next > tau) {
	    // cerr << "buffer: end of high water mark: " << this->dump(prime) << endl;
	    break;		// past our time, leave for later
	}
	m_buffer.insert(prime);

    }
    // cerr << "buffer: buffering " << m_buffer.size() << " to tau=" << tau << endl;
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
	// cerr << "DepoTransport returning NULL pointer!" << endl;
	return 0;
    }
    IDepoPtr before = pop();
    IDepoPtr ret(new TransportedDepo(before, m_location, m_drift_velocity));
    // cerr << " buffer size: "  << m_buffer.size() 
    // 	 << " ret: " << this->dump(ret)
    // 	 << endl;
    return ret;
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
    // cerr << "DepoTransport test @" << (void*)this << " buffered:" << m_buffer.size() << endl;
    // if (m_buffer.size() == 0) {
    // 	cerr << "transport buffer is empty" << endl;	
    // }
    // if (m_it == m_end) {
    // 	cerr << "transport stream is dry" << endl;
    // }
    bool empty = ((m_buffer.size() == 0) && (m_it == m_end));
    if (empty) {
    	// cerr << "transport is exhausted"  << endl;
	return false;
    }
    return true;
}

bool DepoTransport::operator==(const DepoTransport& rhs) const
{
    if (this == &rhs) return true;
    if (m_buffer.size() != rhs.m_buffer.size()) return false;
    if (m_it != rhs.m_it) return false;
    return true;
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

