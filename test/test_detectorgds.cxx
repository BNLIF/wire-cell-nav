#include "WCPNav/DetectorGDS.h"

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TH1F.h"
#include "TLine.h"
#include "TMath.h"
#include "TROOT.h"

#include <iostream>
#include <string>
#include <vector>

using namespace WCP;
using namespace std;

int main(int argc, char* argv[])
{

    DetectorGDS gds;
    gds.set_ncryos(1);
    gds.set_napas(0,4);
    Vector center0(-4.0122*units::cm, 15.3431*units::cm, 24.6852*units::cm);
    Vector halves0(3.26512*units::cm, 99.7439*units::cm, 26.7233*units::cm);
    gds.set_apa(0, 0, 45.705*units::deg, 44.274*units::deg, 4.880488*units::cm, 4.880488*units::cm, 4.880488*units::cm, center0, halves0);
    Vector center1(-4.0122*units::cm, -42.2348*units::cm, 77.3702*units::cm);
    Vector halves1(3.26512*units::cm, 42.2504*units::cm, 25.9617*units::cm);
    gds.set_apa(0, 1, 45.705*units::deg, 44.274*units::deg, 4.880488*units::cm, 4.880488*units::cm, 4.880488*units::cm, center1, halves1);
    Vector center2(-4.0122*units::cm, 57.5435*units::cm, 77.3702*units::cm);
    Vector halves2(3.26512*units::cm, 57.5435*units::cm, 25.9617*units::cm);
    gds.set_apa(0, 2, 45.705*units::deg, 44.274*units::deg, 4.880488*units::cm, 4.880488*units::cm, 4.880488*units::cm, center2, halves2);
    Vector center3(-4.0122*units::cm, 15.3431*units::cm, 130.055*units::cm);
    Vector halves3(3.26512*units::cm, 99.7439*units::cm, 26.7235*units::cm);
    gds.set_apa(0, 3, 45.705*units::deg, 44.274*units::deg, 4.880488*units::cm, 4.880488*units::cm, 4.880488*units::cm, center3, halves3);
    gds.buildGDS(); 
    //gds.read_geometry("/home/xiaoyueli/BNLIF/wire-cell/channelwire_35t_cryo0.txt");    
    std::cout<<"built detector"<<std::endl;
    //return;
    gROOT->ProcessLine(".x ~/rootlogon.C");
    
    TCanvas *c = new TCanvas();
    c->Range(-5*units::cm, -90*units::cm, 160*units::cm, 120*units::cm);    
    c->SetFillColorAlpha(0,0);
    TLine *l = new TLine();
    //l->SetLineWidth(0);
    c->cd();

    int colors[] = {2,4,1};
    for (int iplane=0; iplane<3; ++iplane) {
      c->Clear();
      for (short cryo = 0; cryo < gds.ncryos(); cryo++) {
	for (short apa = 0; apa < gds.napa(cryo); apa++) {
	  std::cout<<"before"<<std::endl;
	  const WrappedGDS *apa_gds = gds.get_apaGDS(cryo, apa);
	  std::cout<<"got apa GDS"<<std::endl;
	  WirePlaneType_t plane = (WirePlaneType_t)iplane;
	  GeomWireSelection wip = apa_gds->wires_in_plane(plane);
	  //std::cout<<"\n[CRYO] "<<cryo<<" [APA] "<<apa<<" [PLANE] "<<iplane
	  //	 <<" has "<< wip.size()<<" wires, wire angle is "<<apa_gds->angle(plane)*180/TMath::Pi()<<std::endl;
	  for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
	    const GeomWire& wire = **wit;
	    //for (int index=0; index<(int)wip.size(); ++index) {
	    //const GeomWire* wire = apa_gds->by_planeindex(plane, index);
	    if (wire.face() == 0) continue;
	    const Vector& p1 = wire.point1();
	    const Vector& p2 = wire.point2();
	    std::cout<<wire<<" ("<<p1.x<<","<<p1.y<<","<<p1.z<<") ("<<p2.x<<","<<p2.y<<","<<p2.z<<")\n";
	    l->SetLineColor(colors[iplane]);
	    l->SetLineWidth(0.5*wire.segment() + 1);
	    l->DrawLine(p1.z, p1.y, p2.z, p2.y);
	  }
	}	    
      }
      c->SaveAs(Form("/home/xiaoyueli/BNLIF/wire-cell/nav/test/test_detectorgds_35t_%d.eps",iplane));
    }
    
  
}
