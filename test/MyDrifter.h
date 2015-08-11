#ifndef WIRECELLNAV_TEST_MYDRIFER
#define  WIRECELLNAV_TEST_MYDRIFER

#include "MyDepo.h"
#include <map>
#include <vector>

typedef std::pair<double, double> tpair;
typedef std::pair<double, double> xpair;
typedef std::vector<WireCell::IDepo::pointer> Track;


// Create a track given time and x end points
void make_track(Track& track, const tpair& t, const xpair& x,
		double charge=0.0)
{
    const double tstep = 0.01;

    const double deltat = t.second-t.first;
    const double deltax = x.second-x.first;

    for (double now = t.first; now <= t.second; now += tstep) {
	double frac = (t.second-now)/deltat;
	WireCell::IDepo::pointer p(new MyDepo(now, WireCell::Point(x.first + frac*deltax, 0, 0), charge));
	track.push_back(p);
    }
}

// Sort some depos by time.
struct ByTime {
    bool operator()(const WireCell::IDepo::pointer& lhs, const WireCell::IDepo::pointer& rhs) {
	if (lhs->time() == rhs->time()) {
	    return &lhs < &rhs;
	}
	return lhs->time() < rhs->time();
    }
};


#endif
