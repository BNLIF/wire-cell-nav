#ifndef WIRECELLNAV_TEST_MYDEPO
#define WIRECELLNAV_TEST_MYDEPO

#include "WireCellIface/IDepo.h"

class MyDepo : public WireCell::IDepo {
public:

    MyDepo(double time, const WireCell::Point& pos, double charge = 1.0)
	: m_time(time), m_pos(pos), m_charge(charge) {}

    virtual ~MyDepo() {};
    virtual const WireCell::Point& pos() const { return m_pos; }
    virtual double time() const { return m_time; }
    virtual double charge() const { return m_charge; }

private:
    double m_time, m_charge;
    WireCell::Point m_pos;
};


#endif
