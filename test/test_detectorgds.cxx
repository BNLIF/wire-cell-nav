#include "WireCellNav/DetectorGDS.h"

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TH1F.h"
#include "TLine.h"

#include <iostream>
#include <string>
#include <vector>

using namespace WireCell;
using namespace std;

int main(int argc, char* argv[])
{
    std::vector<std::string> geometry;
    geometry.push_back("/home/xiaoyueli/BNLIF/wire-cell/geom_35t_v5.txt");
    DetectorGDS gds(geometry);
    TCanvas *c = new TCanvas();
    c->Range(-5, -90, 160, 120);    

    TLine *l = new TLine();
    l->SetLineWidth(0);
    c->cd();

    int colors[] = {2,4,1};
    for (short cryo = 0; cryo < gds.ncryos(); cryo++) {
        for (short apa = 0; apa < gds.napa(cryo); apa++) {
	    const WrappedGDS *apa_gds = gds.get_apaGDS(cryo, apa);
	    std::cout<<"got apa gds"<<std::endl;
	    for (int iplane=0; iplane<3; ++iplane) {
	        WirePlaneType_t plane = (WirePlaneType_t)iplane;
		GeomWireSelection wip = apa_gds->wires_in_plane(plane);
		std::cout<<"got wires in plane "<<iplane<<std::endl;
		std::cout<<"angle is "<<apa_gds->angle(plane)<<std::endl;
		for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
		    const GeomWire& wire = **wit;
		    //if (wire.face() == 0) continue;
		    const Vector& p1 = wire.point1();
		    const Vector& p2 = wire.point2();
		    l->SetLineColor(colors[iplane]);
		    l->DrawLine(p1.z, p1.y, p2.z, p2.y);	       
		}
	    }	    
	}
    }
    /*
    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t plane = (WirePlaneType_t)iplane;
	GeomWireSelection wip = gds.wires_in_plane(plane);

	for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
	    const GeomWire& wire = **wit;
	    const Vector& p1 = wire.point1();
	    const Vector& p2 = wire.point2();

	    l->SetLineColor(colors[iplane]);
	    l->DrawLine(p1.z, p1.y, p2.z, p2.y);

	}
    }
    */

    
    c->SaveAs("/home/xiaoyueli/BNLIF/wire-cell/nav/test_detectorgds_35t.pdf");
  
}
