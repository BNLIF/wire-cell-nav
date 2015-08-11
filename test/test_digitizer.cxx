#include "WireCellNav/WireParams.h"
#include "WireCellNav/ParamWires.h"
#include "WireCellNav/WireSummary.h"
#include "WireCellNav/Drifter.h"
#include "WireCellNav/Digitizer.h"
#include "WireCellUtil/RangeFeed.h"

#include "WireCellUtil/Testing.h"
#include "WireCellUtil/TimeKeeper.h"

#include "MyDepo.h"
#include "MyDrifter.h"


using namespace WireCell;
using namespace std;

const double drift_velocity = 1.0;


std::string dump(IFrame::pointer frame)
{
    stringstream ss;
    ss << "frame: " << frame->ident() << " @" << frame->time() << "s with "
       << boost::distance(frame->range()) << " traces\n";
    for (auto it : frame->range()) {
	ss << "\tchid=" << it->channel() << " tbin=" << it->tbin() << " bins=" << it->charge().size() << "\n";
    }
    return ss.str();
}

int main() {
    TimeKeeper tk("test_digitizer");
    Track activity;
    make_track_4d(activity, tpair(10,11), Ray(Point(10,0,0), Point(10,10,10)), 1);
    make_track_4d(activity, tpair(12,13), Ray(Point( 1,0,0), Point( 2,-10,0)), 2);
    make_track_4d(activity, tpair( 9,10), Ray(Point(13,5,5), Point(11, -5,-3)), 3);

    cout << tk("make tracks") << endl;
    sort(activity.begin(), activity.end(), ByTime());
    cout << tk("sorted tracks") << endl;


    WireParams wp;
    ParamWires pw;
    pw.generate(wp);
    IWireSummary::pointer ws(new WireSummary);
    AssertMsg(ws, "Null wire summary");

    IWireSink::pointer wsink = dynamic_pointer_cast<IWireSink>(ws);
    AssertMsg(wsink, "Null cast to wire sink");
    wsink->sink(pw.wires_range());

    RangeFeed<Track::iterator> feed(activity.begin(), activity.end());
    Drifter drifter(0, drift_velocity);
    drifter.connect(feed);
    Digitizer digitizer;
    digitizer.set_wiresummary(ws);
    digitizer.connect(drifter);

    IFrame::pointer p;
    int count = 0;
    while ((p=digitizer())) {
	AssertMsg(p,"got null frame");
	cout << "Dumping frame: #" << count << " " << dump(p) << endl;
	++count;
    }

    return 0;
}
