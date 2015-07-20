#include "WireCellNav/ParamGDS.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLine.h"
#include "TMarker.h"

#include <iostream>
#include <cassert>

using namespace WireCell;
using namespace std;

int main(int argc, char* argv[])
{
    float dx=10*units::mm;
    float dy=1*units::meter;
    float dz=1*units::meter;
    float pitch = 10.0*units::mm;

    float angU = 60.0*units::pi/180.0;
    float angV = M_PI-angU;
    float angW = 0.0;

    const Vector center;
    const Vector deltabb(dx,dy,dz);
    const Vector bmin = center - 0.5*deltabb;
    const Vector bmax = center + 0.5*deltabb;

    // pitch vectors
    const Vector pU(0, pitch*std::sin(+angU), pitch*std::cos(+angU));
    const Vector pV(0, pitch*std::sin(+angV), pitch*std::cos(+angV));
    const Vector pW(0, pitch*std::sin(+angW), pitch*std::cos(+angW));

    // offset/starting points
    const Vector oU(+0.25*dx, 0.0, 0.0);
    const Vector oV( 0.0    , 0.0, 0.0);
    const Vector oW(-0.25*dx, 0.0, 0.0);

    ParamGDS gds(bmin,bmax, oU,pU, oV,pV, oW,pW);
    if (!gds.get_wires().size()) {
	return 2;
    }

    int ntot = gds.get_wires().size();

    TApplication* theApp = 0;
    if (argc > 1) {
        theApp = new TApplication ("test_root_wires",0,0);
    }
    TCanvas c;
    TLine l;
    TMarker m;
    TH1F* frame = c.DrawFrame(bmin.z, bmin.y, bmax.z, bmax.y);
    frame->SetXTitle("transverse Z direction");
    frame->SetYTitle("transverse Y direction");

    int colors[] = {1,2,4};

    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t plane = (WirePlaneType_t)iplane;
	GeomWireSelection wip = gds.wires_in_plane(plane);

	for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
	    const GeomWire& wire = **wit;
	    const Vector p1 = wire.point1();
	    const Vector p2 = wire.point2();

	    l.SetLineColor(colors[iplane]);
	    l.DrawLine(p1.z, p1.y, p2.z, p2.y);

	    Point cent = 0.5*(p1+p2);
	    m.SetMarkerSize(5);
	    m.DrawMarker(cent.z, cent.y);
	}
    }

    if (theApp) {
        theApp->Run();
    }
    else {                      // batch
        c.Print("test_root_wires.pdf");
    }


    return 0;
}
