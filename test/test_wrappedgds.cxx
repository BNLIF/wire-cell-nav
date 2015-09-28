#include "WireCellNav/WrappedGDS.h"
#include "WireCellData/Point.h"

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
    const Point pt1(10,0,0);    
    const Point pt2(10,0,5.5);
    WrappedGDS gds(bmin, bmax, 35.0/180.0*M_PI, 10);
    
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
	std::cout<<"# of wires is "<<wip.size()<<std::endl;
	for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
	    const GeomWire& wire = **wit;
	    const Vector& p1 = wire.point1();
	    const Vector& p2 = wire.point2();

	    l.SetLineColor(colors[iplane]);
	    l.DrawLine(p1.z, p1.y, p2.z-offset, p2.y-offset);

	}
	std::pair<double, double> minmax_x = gds.minmax(0);
	std::cout<<"plane "<<iplane<<"; min max along x-axis is "<<minmax_x.first<<", "<<minmax_x.second<<std::endl;
	std::pair<double, double> minmax_px = gds.minmax(0, plane);
	std::cout<<"       "<<"; min max along x-axis is "<<minmax_px.first<<", "<<minmax_px.second<<std::endl;
	std::pair<double, double> minmax_y = gds.minmax(1);
	std::cout<<"       "<<"; min max along y-axis is "<<minmax_y.first<<", "<<minmax_y.second<<std::endl;
	std::pair<double, double> minmax_py = gds.minmax(1, plane);
	std::cout<<"       "<<"; min max along y-axis is "<<minmax_py.first<<", "<<minmax_py.second<<std::endl;
	std::pair<double, double> minmax_z = gds.minmax(2);
	std::cout<<"       "<<"; min max along z-axis is "<<minmax_z.first<<", "<<minmax_z.second<<std::endl;
	std::pair<double, double> minmax_pz = gds.minmax(2, plane);
	std::cout<<"       "<<"; min max along z-axis is "<<minmax_pz.first<<", "<<minmax_pz.second<<std::endl;
	std::cout<<"       "<<"; angle is "<<gds.angle(plane)<<std::endl;
    }

    
    if (theApp) {
        theApp->Run();
    }
    else {                      // batch
        c.Print("test_wrappedgds.pdf");
    }


}
