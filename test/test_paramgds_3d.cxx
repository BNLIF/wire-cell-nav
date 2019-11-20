#include "WCPNav/ParamGDS.h"
#include "WCPData/Units.h"


#include "TApplication.h"
#include "TCanvas.h"
#include "TView.h"
#include "TPolyLine3D.h"

#include <iostream>
#include <cmath>
#include <cassert>

using namespace WCP;
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
    const Vector pU(0, pitch*std::cos(+angU), pitch*std::sin(+angU));
    const Vector pV(0, pitch*std::cos(+angV), pitch*std::sin(+angV));
    const Vector pW(0, pitch*std::cos(+angW), pitch*std::sin(+angW));

    // offset/starting points
    const Vector oU(+0.25*dx, 0.0, 0.0);
    const Vector oV( 0.0    , 0.0, 0.0);
    const Vector oW(-0.25*dx, 0.0, 0.0);

    cerr << "bmin=" << bmin << " bmax=" << bmax << endl;
    cerr << "Uoffset=" << oU << " Upitch=" << pU << endl;
    cerr << "Voffset=" << oV << " Vpitch=" << pV << endl;
    cerr << "Woffset=" << oW << " Wpitch=" << pW << endl;

    ParamGDS gds(bmin,bmax, oU,pU, oV,pV, oW,pW);
    int ntot = gds.get_wires().size();
    cerr << "#wires = " << ntot << endl;
    //assert(ntot == 32);


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
        c.Print("test_paramgds_3d.pdf");
    }

    return 0;
}
