#ifndef WIRECELLNAV_TEST_MYDEPO
#define WIRECELLNAV_TEST_MYDEPO

#include "WireCellIface/IDepo.h"

class MyDepo : public WireCell::IDepo {
public:

    MyDepo(double time, const WireCell::Point& pos = WireCell::Point(), double charge = 1.0)
	: m_time(time), m_pos(pos), m_charge(charge) {}

    virtual ~MyDepo() {};
    virtual const WireCell::Point& pos() const { return m_pos; }
    virtual double time() const { return m_time; }
    virtual double charge() const { return m_charge; }
    virtual WireCell::IDepo::pointer prior() const { return 0; } // primordial

private:
    double m_time, m_charge;
    WireCell::Point m_pos;
};

class SimpleDepo : public WireCell::IDepo {
    double m_time;
    WireCell::Point m_pos;
public:
    SimpleDepo(double t, const WireCell::Point& pos) : m_time(t), m_pos(pos)  {}

    virtual const WireCell::Point& pos() const { return m_pos; }
    virtual double time() const { return m_time; }
    virtual double charge() const { return 1.0; }
    virtual WireCell::IDepo::pointer prior() const { return 0; } // primordial
};


bool by_time_desc(const WireCell::IDepo::pointer& lhs, const WireCell::IDepo::pointer& rhs)
{
    if (lhs->time() == rhs->time()) {
	return lhs.get() > rhs.get();
    }
    return lhs->time() > rhs->time();
}


struct TrackDepos {

    const double stepsize;
    const double clight;
    std::shared_ptr<WireCell::IDepoVector> depos;
    
    TrackDepos(double stepsize=0.1*units::millimeter,
	       double clight=1.0*units::meter/units::microsecond)
	: stepsize(stepsize), clight(clight), depos(new WireCell::IDepoVector) {}

    void add_track(double time, const WireCell::Ray& ray) {

	const WireCell::Vector dir = WireCell::ray_unit(ray);
	const double length = WireCell::ray_length(ray);
	double step = 0;
	int count = 0;
	while (step < length) {
	    const double now = time + step/clight;
	    const WireCell::Point here = ray.first + dir * step;
	    //cerr << "\tdepo: " << here << " @" << now << endl;
	    depos->push_back(WireCell::IDepo::pointer(new SimpleDepo(now, here)));
	    step += stepsize;
	    ++count;
	}

	std::cerr << "add_track(t=" << time << ", pos=" << ray.first << " --> " << ray.second << ") " 
		  << count << " deposits, step=" << step << " length=" << length << std::endl;
	std::cerr << "\tlast: t=" << depos->back()->time() << " pos=" << depos->back()->pos() << std::endl;

	std::sort(depos->begin(), depos->end(), by_time_desc);
    }

    int left() { return depos->size(); };

    WireCell::IDepo::pointer operator()() {
	if (!depos->size()) { return nullptr; }
	WireCell::IDepo::pointer p = depos->back();
	depos->pop_back();
	//cerr << "Popping t=" << p->time() << ", pos=" << p->pos() << " with left: " << left() << endl;
	return p;
    }

    WireCell::IDepoVector& depositions() { return *depos.get(); }
};

#endif
