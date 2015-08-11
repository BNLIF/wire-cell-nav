#ifndef WIRECELLNAV_TEST_MYDRIFER
#define  WIRECELLNAV_TEST_MYDRIFER

#include "WireCellUtil/Point.h"

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

// Make a track going along ray at constant speed spanning given time
void make_track_4d(Track& track, const tpair& dt, const WireCell::Ray& ray,
		   double charge=0.0)
{
    const double tstep = 0.01;
    const double deltat = dt.second - dt.first;

    const double nsteps = deltat/tstep;
    const WireCell::Point step = WireCell::ray_vector(ray) * (1.0/nsteps);

    for (double istep = 0; istep < nsteps; ++istep) {
	const WireCell::Point p = istep*step + ray.first;
	const double now = istep*deltat + dt.first;
	WireCell::IDepo::pointer depo(new MyDepo(now, p, charge));
	track.push_back(depo);
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
