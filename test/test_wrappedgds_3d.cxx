#include "WireCellNav/WrappedGDS.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TView.h"
#include "TPolyLine3D.h"

#include <cmath>
#include <iostream>
using namespace WireCell;
using namespace std;

int main(int argc, char* argv[])
{
    const Vector bmin(-5,-600,-300);
    const Vector bmax(+5,+600,+300);

    WrappedGDS gds(bmin, bmax, 35.0/180.0*M_PI, 30);
    
    TApplication* theApp = 0;
    if (argc > 1) {
        theApp = new TApplication ("test_wrappedgds",0,0);
    }
    TCanvas c;
    TView* view = TView::CreateView(1);
    view->SetRange(bmin.x,bmin.y,bmin.z, bmax.x,bmax.y,bmax.z);
    view->ShowAxis();

    int colors[] = {2,4,1};
    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t plane = (WirePlaneType_t)iplane;
	GeomWireSelection wip = gds.wires_in_plane(plane);

	for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
	    const GeomWire& wire = **wit;
	    const Vector& p1 = wire.point1();
	    const Vector& p2 = wire.point2();

	    cerr << wire << " " << p1 << " " << p2 << endl;

	    TPolyLine3D* l = new TPolyLine3D(2);
	    l->SetLineColor(colors[iplane]);
	    l->SetLineWidth(2*wire.segment() + 1);
	    l->SetPoint(0, p1.x,p1.y,p1.z);
	    l->SetPoint(1, p2.x,p2.y,p2.z);
	    l->Draw();
	}
    }

    cout << "U=red, V=blue, W=black, green axis is Y-axis" << endl;

    if (theApp) {
        theApp->Run();
    }
    else {                      // batch
        c.Print("test_wrappedgds_3d.pdf");
    }


}
