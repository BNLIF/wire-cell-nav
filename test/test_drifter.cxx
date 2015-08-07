#include "WireCellNav/Drifter.h"

#include "WireCellUtil/RangeFeed.h"
#include "WireCellUtil/Testing.h"
#include "WireCellUtil/TimeKeeper.h"

#include "MyDepo.h"

#include <map>
#include <iostream>
#include <sstream>

using namespace WireCell;
using namespace std;

typedef std::pair<double, double> tpair;
typedef std::pair<double, double> xpair;
typedef std::vector<IDepo::pointer> Track;

const double drift_velocity = 1.0;



void make_track(Track& track, const tpair& t, const xpair& x,
		double charge=0.0)
{
    const double tstep = 0.01;

    const double deltat = t.second-t.first;
    const double deltax = x.second-x.first;

    for (double now = t.first; now <= t.second; now += tstep) {
	double frac = (t.second-now)/deltat;
	IDepo::pointer p(new MyDepo(now, Point(x.first + frac*deltax, 0, 0), charge));
	track.push_back(p);
    }
}

std::string dump(const IDepo::pointer& p)
{
    stringstream ss;
    double x = p->pos().x(), t = p->time(), tau = t+x/drift_velocity;
    ss << "<IDepo tau:" << tau << " t:" << t << " x:" << x << " q:" << p->charge() << ">";
    return ss.str();
}

struct ByTime {
    bool operator()(const IDepo::pointer& lhs, const IDepo::pointer& rhs) {
	if (lhs->time() == rhs->time()) {
	    return &lhs < &rhs;
	}
	return lhs->time() < rhs->time();
    }
};

int test_sort(Track& activity)
{
    int norig = activity.size();
    cout << "Starting with " << norig << " depositions" << endl;

    sort(activity.begin(), activity.end(), ByTime());
    AssertMsg (norig == activity.size(), "Sort lost depos");

    int nsorted = 0;
    for (auto tit = activity.begin(); tit != activity.end(); ++tit) {
    	cout << "sorted: #" << nsorted << " " << dump(*tit) << endl;
	++nsorted;
    }
    return nsorted;
}

int test_feed(Track& activity)
{
    int count=0, norig = activity.size();
    RangeFeed<Track::iterator> feed(activity.begin(), activity.end());
    IDepo::pointer p;
    while ((p=feed())) {
	cout << "feed: #" << count << " " << dump(p) << endl;
	++count;
    }
    AssertMsg(count == norig , "Lost some points from feed"); 

}
int test_drifted(Track& activity)
{
    int count=0, norig = activity.size();
    RangeFeed<Track::iterator> feed(activity.begin(), activity.end());
    Drifter drifter(0, drift_velocity);
    drifter.connect(feed);

    IDepo::pointer p;
    while ((p=drifter())) {
	cout << "drift: #" << count << " " << dump(p) << endl;
	++count;
    }
    AssertMsg(count == norig , "Lost some points drifting"); 
}

int main()
{
    TimeKeeper tk("test_depotransport");
    Track activity;
    make_track(activity, tpair(10,11), xpair(10,11), 1);
    make_track(activity, tpair(12,13), xpair(1,2), 2);// close but late
    make_track(activity, tpair(9,10), xpair(13,11), 3);// far but early
    make_track(activity, tpair(9.5,11.5), xpair(10.5,11.5), 4);// overlapping
    cout << tk("make tracks") << endl;
    
    test_sort(activity);	// side effects needed by all tests
    cout << tk("sorted") << endl;
    test_feed(activity);
    cout << tk("collection") << endl;
    test_drifted(activity);
    cout << tk("transport") << endl;
    cout << tk.summary() << endl;
    return 0;
}
