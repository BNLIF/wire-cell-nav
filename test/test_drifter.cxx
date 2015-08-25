#include "WireCellNav/Drifter.h"

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
#include "TColor.h"


#include <iostream>
#include <sstream>

using namespace WireCell;
using namespace std;


int main(int argc, char* argv[])
{
    TimeKeeper tk("test_drifter");
    Track activity;
    // make_track_4d(activity, tpair(10,11), Ray(Point(10,0,0), Point(10,10,10)), 1);
    // make_track_4d(activity, tpair(12,13), Ray(Point( 1,0,0), Point( 2,-10,0)), 2);
    // make_track_4d(activity, tpair( 9,10), Ray(Point(13,5,5), Point(11, -5,-3)), 3);
    make_track_4d(activity, 10, Ray(Point(10,0,0), Point(100,10,10)), 1);
    make_track_4d(activity, 120, Ray(Point( 1,0,0), Point( 2,-100,0)), 2);
    make_track_4d(activity, 99, Ray(Point(130,50,50), Point(11, -50,-30)), 3);

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
    TPolyMarker3D orig(activity.size(), 6);
    orig.SetMarkerColor(2);
    int indx=0;
    for (auto depo : activity) {
	const Point& p = depo->pos();
	orig.SetPoint(indx++, p.x(), p.y(), p.z());
    }
    orig.Draw();

    // draw drifted
    double tmin=-1, tmax=-1;
    for (auto depo : drifted) {
	auto history = depo_chain(depo);
	Assert(history.size() > 1);

	if (tmin<0 && tmax<0) {
	    tmin = tmax = depo->time();
	    continue;
	}
	tmin = min(tmin, depo->time());
	tmax = max(tmax, depo->time());
    }
    cerr << "Time bounds: " << tmin << " < " << tmax << endl;

    for (auto depo : drifted) {

	TPolyMarker3D* pm = new TPolyMarker3D(1,8);
	const Point& p = depo->pos();
	pm->SetPoint(0, p.x(), p.y(), p.z());

	double rel = depo->time()/(tmax-tmin);
	int col = TColor::GetColorPalette( int(rel*TColor::GetNumberOfColors()) );
	pm->SetMarkerColor(col);

	pm->Draw();
    }

    if (theApp) {
	theApp->Run();
    }
    else {			// batch
	c.Print("test_drifter.pdf");
    }

    return 0;
}
