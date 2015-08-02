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

private:
    double m_time, m_charge;
    WireCell::Point m_pos;
};


class MyDepoSrc : virtual public WireCell::IDepoSource
{
    int m_count, m_max;
public:
    MyDepoSrc(int max = 10) : m_count(0), m_max(max) {}
    virtual ~MyDepoSrc() {}
    virtual WireCell::IDepo::const_ptr depo_gen() {
	++m_count;
	if (m_count >= m_max) { return nullptr; }
	return WireCell::IDepo::const_ptr(new MyDepo(m_count));
    }
};

class MyDepoFilter : virtual public WireCell::IDepoSource
{
    WireCell::IDepoSource& m_src;
    int m_mod;
public:
    MyDepoFilter(WireCell::IDepoSource& src, int mod=2) : m_src(src), m_mod(mod) {}
    virtual ~MyDepoFilter() {}
    virtual WireCell::IDepo::const_ptr depo_gen() {
	WireCell::IDepo::const_ptr p;
	while ((p=m_src.depo_gen())) {
	    if (int(p->time()) % m_mod == 0) {
		return p;
	    }
	}
	return nullptr;
    }
};
#endif
