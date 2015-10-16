#include "WireCellNav/DetectorGDS.h"

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TH1F.h"
#include "TLine.h"
#include "TMath.h"

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
    c->Range(-5*units::cm, -90*units::cm, 160*units::cm, 120*units::cm);    

    TLine *l = new TLine();
    l->SetLineWidth(0);
    c->cd();

    int colors[] = {2,4,1};
    for (short cryo = 0; cryo < gds.ncryos(); cryo++) {
      for (short apa = 0; apa < gds.napa(cryo); apa++) {
	    const WrappedGDS *apa_gds = gds.get_apaGDS(cryo, apa);
	    for (int iplane=0; iplane<3; ++iplane) {
	        WirePlaneType_t plane = (WirePlaneType_t)iplane;
		GeomWireSelection wip = apa_gds->wires_in_plane(plane);
	        std::cout<<"\n[CRYO] "<<cryo<<" [APA] "<<apa<<" [PLANE] "<<iplane
			 <<" has "<< wip.size()<<" wires, wire angle is "<<apa_gds->angle(plane)*180/TMath::Pi()<<std::endl;
		//for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
		//  const GeomWire& wire = **wit;
		for (int index=0; index<(int)wip.size(); ++index) {
		    const GeomWire* wire = apa_gds->by_planeindex(plane, index);
		    //if (wire.face() == 0) continue;
		    const Vector& p1 = wire->point1();
		    const Vector& p2 = wire->point2();
		    std::cout<<*wire<<" ("<<p1.x<<","<<p1.y<<","<<p1.z<<") ("<<p2.x<<","<<p2.y<<","<<p2.z<<")\n";
		    l->SetLineColor(colors[iplane]);
		    l->DrawLine(p1.z, p1.y, p2.z, p2.y);
		}
	    }	    
	}
    }
    
    c->SaveAs("/home/xiaoyueli/BNLIF/wire-cell/nav/test/test_detectorgds_35t.pdf");
  
}
