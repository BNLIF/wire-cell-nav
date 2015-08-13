// Note: this is just to share some code between tests.  It shouldn't
// be included elsewhere.

#ifndef WIRECELLNAV_TEST_MYDRIFER
#define  WIRECELLNAV_TEST_MYDRIFER

#include "WireCellUtil/Point.h"
#include "WireCellUtil/RangeFeed.h"

#include "MyDepo.h"

#include <map>
#include <vector>

typedef std::pair<double, double> tpair;
typedef std::pair<double, double> xpair;
typedef std::vector<WireCell::IDepo::pointer> Track;


// some bogus time, speed, distance parameters
const double drift_velocity = 1.0; // one unit distance per one unit time
const double speed_of_light = 100.0;
const double tick_time = 1.0;

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
void make_track_4d(Track& track, double start_time, const WireCell::Ray& ray,
		   double charge=0.0)
{
    const double tstep = 0.01;
    const WireCell::Point tvec = WireCell::ray_vector(ray);
    const double deltat = tvec.magnitude()/speed_of_light;
    const double nsteps = deltat/tstep;
    const WireCell::Point step = tvec * (1.0/nsteps);

    for (double istep = 0; istep < nsteps; ++istep) {
	const WireCell::Point p = istep*step + ray.first;
	const double now = istep*deltat + start_time;
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

int test_sort(Track& activity)
{
    int norig = activity.size();

    sort(activity.begin(), activity.end(), ByTime());
    AssertMsg (norig == activity.size(), "Sort lost depos");

    int nsorted = 0;
    for (auto tit = activity.begin(); tit != activity.end(); ++tit) {
	++nsorted;
    }
    return nsorted;
}

int test_feed(Track& activity)
{
    int count=0, norig = activity.size();
    WireCell::RangeFeed<Track::iterator> feed(activity.begin(), activity.end());
    WireCell::IDepo::pointer p;
    while ((p=feed())) {
	++count;
    }
    AssertMsg(count == norig , "Lost some points from feed"); 

}
int test_drifted(Track& activity, Track& result)
{
    int count=0, norig = activity.size();
    WireCell::RangeFeed<Track::iterator> feed(activity.begin(), activity.end());
    WireCell::Drifter drifter(0, drift_velocity);
    drifter.connect(feed);

    WireCell::IDepo::pointer p;
    while ((p=drifter())) {
	result.push_back(p);
	++count;
    }
    AssertMsg(count == norig , "Lost some points drifting"); 
}


#endif
