#include "WireCellNav/DepoTransport.h"
#include "WireCellUtil/Testing.h"
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
    const double tstep = 0.1;

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
	return lhs->time() < rhs->time();
    }
};

int main()
{

    std::vector<IDepoPtr> activity;
    make_track(activity, tpair(10,11), xpair(10,11), 1);
    make_track(activity, tpair(12,13), xpair(1,2), 2);// close but late
    make_track(activity, tpair(9,10), xpair(13,11), 3);// far but early
    make_track(activity, tpair(9.5,11.5), xpair(10.5,11.5), 4);// overlapping
    sort(activity.begin(), activity.end(), ByTime());


    cout << "Starting with " << activity.size() << " depositions" << endl;

    for (auto tit = activity.begin(); tit != activity.end(); ++tit) {
    	cout << "raw: " << dump(*tit) << endl;
    }

    typedef IteratorAdapter< std::vector<IDepoPtr>::iterator, depoptr_base_iterator > adapted_iter;
    adapted_iter abeg(activity.begin()), aend(activity.end());

    // cout << endl;
    // {
    // 	DepoTransport dt(abeg, aend, 0, drift_velocity);
    // 	while (dt) {
    // 	    IDepoPtr p = dt();
    // 	    cout << "dir: " << dump(p) << endl;
    // 	}
    // }

    depoptr_range transported = depo_transport(abeg, aend, 0, drift_velocity);

    cout << endl;
    for (auto dit = transported.first; dit != transported.second; ++dit) {
    	IDepoPtr p(*dit);
    	cout << "itr: " << dump(p) << endl;
    }

    return 0;
}
