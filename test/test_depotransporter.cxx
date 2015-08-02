#include "WireCellNav/DepoCollection.h"
#include "WireCellNav/DepoTransporter.h"
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

const double drift_velocity = 1.0;


void make_track(std::vector<IDepoPtr>& track, const tpair& t, const xpair& x, double charge=0.0)
{
    const double tstep = 0.01;

    const double deltat = t.second-t.first;
    const double deltax = x.second-x.first;

    for (double now = t.first; now <= t.second; now += tstep) {
	double frac = (t.second-now)/deltat;
	IDepoPtr p(new MyDepo(now, Point(x.first + frac*deltax, 0, 0), charge));
	track.push_back(p);
    }
}

std::string dump(const IDepoPtr& p)
{
    stringstream ss;
    double x = p->pos().x(), t = p->time(), tau = t+x/drift_velocity;
    ss << "<IDepo tau:" << tau << " t:" << t << " x:" << x << " q:" << p->charge() << ">";
    return ss.str();
}

struct ByTime {
    bool operator()(const IDepoPtr& lhs, const IDepoPtr& rhs) {
	if (lhs->time() == rhs->time()) {
	    return &lhs < &rhs;
	}
	return lhs->time() < rhs->time();
    }
};

typedef IteratorAdapter< std::vector<IDepoPtr>::iterator, depoptr_base_iterator > adapted_iter;

int test_sort(std::vector<IDepoPtr>& activity)
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

int test_collection(std::vector<IDepoPtr>& activity)
{
    int ncol=0, norig = activity.size();
    DepoCollection dc(depoptr_range(adapted_iter(activity.begin()),
				    adapted_iter(activity.end())));
    IDepo::const_ptr p;
    while ((p=dc.depo_gen())) {
	cout << "col: #" << ncol << " " << dump(p) << endl;
	++ncol;
    }
    AssertMsg(ncol == norig , "Lost some points from collection"); 
}

int test_transport(std::vector<IDepoPtr>& activity)    
{
    int ntrans = 0, norig = activity.size();

    DepoCollection dc(depoptr_range(adapted_iter(activity.begin()),
				    adapted_iter(activity.end())));
    DepoTransporter dt(dc, 0, drift_velocity);
    IDepo::const_ptr p;
    while ((p=dt.depo_gen())) {
	cout << "trans: #" << ntrans << " " << dump(p) << endl;
	++ntrans;
    }
    AssertMsg(ntrans == norig , "Lost some points from transport"); 
}

int main()
{
    TimeKeeper tk("test_depotransport");
    std::vector<IDepoPtr> activity;
    make_track(activity, tpair(10,11), xpair(10,11), 1);
    make_track(activity, tpair(12,13), xpair(1,2), 2);// close but late
    make_track(activity, tpair(9,10), xpair(13,11), 3);// far but early
    make_track(activity, tpair(9.5,11.5), xpair(10.5,11.5), 4);// overlapping
    cout << tk("make tracks") << endl;
    
    test_sort(activity);	// side effects needed by all tests
    cout << tk("sorted") << endl;
    test_collection(activity);
    cout << tk("collection") << endl;
    test_transport(activity);
    cout << tk("transport") << endl;
    cout << tk.summary() << endl;
    return 0;
}
