#include "WireCellNav/WrappedGDS.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLine.h"
#include "TMarker.h"

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
    TLine l;
    TMarker m;
    TH1F* frame = c.DrawFrame(bmin.z, bmin.y, bmax.z, bmax.y);
    frame->SetXTitle("transverse Z direction");
    frame->SetYTitle("transverse Y direction");


    const double offset = 0.0;

    int colors[] = {2,4,1};
    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t plane = (WirePlaneType_t)iplane;
	GeomWireSelection wip = gds.wires_in_plane(plane);

	for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
	    const GeomWire& wire = **wit;
	    const Vector& p1 = wire.point1();
	    const Vector& p2 = wire.point2();

	    l.SetLineColor(colors[iplane]);
	    l.DrawLine(p1.z, p1.y, p2.z-offset, p2.y-offset);

	}
    }

    if (theApp) {
        theApp->Run();
    }
    else {                      // batch
        c.Print("test_wrappedgds.pdf");
    }


}
