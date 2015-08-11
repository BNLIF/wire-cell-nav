#include "WireCellNav/Drifter.h"

#include "WireCellUtil/RangeFeed.h"
#include "WireCellUtil/Testing.h"
#include "WireCellUtil/TimeKeeper.h"

#include "MyDrifter.h"

#include <iostream>
#include <sstream>

using namespace WireCell;
using namespace std;

const double drift_velocity = 1.0;




std::string dump(const IDepo::pointer& p)
{
    stringstream ss;
    double x = p->pos().x(), t = p->time(), tau = t+x/drift_velocity;
    ss << "<IDepo tau:" << tau << " t:" << t << " x:" << x << " q:" << p->charge() << ">";
    return ss.str();
}

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
    TimeKeeper tk("test_drifter");
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
