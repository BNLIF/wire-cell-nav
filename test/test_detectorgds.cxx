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
    //geometry.push_back("/home/xiaoyueli/BNLIF/wire-cell/geom_35t_v5.txt");
    //DetectorGDS gds(geometry);
    
    DetectorGDS gds;
    gds.set_ncryos(1);
    gds.set_napas(0,4);
    Vector center0(-4.0122, 15.3431, 24.6852);
    Vector halves0(3.26512, 99.7439, 26.7233);
    gds.set_apa(0, 0, 45.705, 44.274, 0.4880488, 0.4880488, 0.4880488, center0, halves0);
    Vector center1(-4.0122, -42.2348, 77.3702);
    Vector halves1(3.26512, 42.2504, 25.9617);
    gds.set_apa(0, 1, 45.705, 44.274, 0.4880488, 0.4880488, 0.4880488, center1, halves1);
    Vector center2(-4.0122, 57.5435, 77.3702);
    Vector halves2(3.26512, 57.5435, 25.9617);
    gds.set_apa(0, 2, 45.705, 44.274, 0.4880488, 0.4880488, 0.4880488, center2, halves2);
    Vector center3(-4.0122, 15.3431, 130.055);
    Vector halves3(3.26512, 99.7439, 26.7235);
    gds.set_apa(0, 3, 45.705, 44.274, 0.4880488, 0.4880488, 0.4880488, center3, halves3);
    gds.buildGDS();

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
