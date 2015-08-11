#include "WireCellNav/Drifter.h"

#include "WireCellUtil/RangeFeed.h"
#include "WireCellUtil/Testing.h"
#include "WireCellUtil/TimeKeeper.h"
#include "WireCellUtil/BoundingBox.h"

#include "MyDrifter.h"

#include "TApplication.h"
#include "TCanvas.h"

// 3d
#include "TView.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"


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
    	//cout << "sorted: #" << nsorted << " " << dump(*tit) << endl;
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
	//cout << "feed: #" << count << " " << dump(p) << endl;
	++count;
    }
    AssertMsg(count == norig , "Lost some points from feed"); 

}
int test_drifted(Track& activity, Track& result)
{
    int count=0, norig = activity.size();
    RangeFeed<Track::iterator> feed(activity.begin(), activity.end());
    Drifter drifter(0, drift_velocity);
    drifter.connect(feed);

    IDepo::pointer p;
    while ((p=drifter())) {
	//cout << "drift: #" << count << " " << dump(p) << endl;
	result.push_back(p);
	++count;
    }
    AssertMsg(count == norig , "Lost some points drifting"); 
}

int main(int argc, char* argv[])
{
    TimeKeeper tk("test_drifter");
    Track activity;
    make_track_4d(activity, tpair(10,11), Ray(Point(10,0,0), Point(10,10,10)), 1);
    make_track_4d(activity, tpair(12,13), Ray(Point( 1,0,0), Point( 2,-10,0)), 2);
    make_track_4d(activity, tpair( 9,10), Ray(Point(13,5,5), Point(11, -5,-3)), 3);
    cout << tk("make tracks") << endl;
    

    BoundingBox bbox(Ray(Point(-1,-1,-1), Point(1,1,1)));
    for (auto depo : activity) {
	bbox(depo->pos());
    }
    Ray bb = bbox.bounds();
    cout << "Bounds: " << bb.first << " --> " << bb.second << endl;

    test_sort(activity);	// side effects needed by all tests
    cout << tk("sorted") << endl;
    test_feed(activity);
    cout << tk("collection") << endl;
    Track drifted;
    test_drifted(activity, drifted);
    cout << tk("transport") << endl;
    cout << tk.summary() << endl;

    
    TApplication* theApp = 0;
    if (argc > 1) {
	theApp = new TApplication ("test_drifter",0,0);
    }

    TCanvas c("c","c",800,800);

    TView* view = TView::CreateView(1);
    view->SetRange(bb.first.x(),bb.first.y(),bb.first.z(),
		   bb.second.x(),bb.second.y(),bb.second.z());
    view->ShowAxis();


    // draw raw activity
    TPolyMarker3D orig(activity.size(), 1);
    orig.SetMarkerColor(2);
    int indx=0;
    for (auto depo : activity) {
	const Point& p = depo->pos();
	orig.SetPoint(indx++, p.x(), p.y(), p.z());
    }
    orig.Draw();

    // draw drifted
    TPolyMarker3D drifm(drifted.size(), 1);
    orig.SetMarkerColor(4);
    indx=0;
    for (auto depo : drifted) {
	const Point& p = depo->pos();
	drifm.SetPoint(indx++, p.x(), p.y(), p.z());
    }
    drifm.Draw();

    if (theApp) {
	theApp->Run();
    }
    else {			// batch
	c.Print("test_drifter.pdf");
    }

    return 0;
}
