// Private class for DepoTransport.

#ifndef WIRECELL_NAV_TRANSPORTEDDEPO
#define WIRECELL_NAV_TRANSPORTEDDEPO

class TransportedDepo : public WireCell::IDepo {
    WireCell::IDepo::const_ptr m_from;
    WireCell::Point m_pos;
    double m_time;
public:

    TransportedDepo(const WireCell::IDepo::const_ptr& from, double location, double velocity) 
	: m_from(from), m_pos(from->pos()) {
	double dx = m_pos.x() - location;
	m_pos.x(location);
	m_time = from->time() + dx/velocity;
    }
    virtual ~TransportedDepo() {};

    virtual const WireCell::Point& pos() const { return m_pos; }
    virtual double time() const { return m_time; }
    virtual double charge() const { return m_from->charge(); }
    virtual WireCell::IDepo::const_ptr original() { return m_from; }
};


#endif
