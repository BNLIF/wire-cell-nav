#include "WireCellNav/DetectorGDS.h"

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TH1F.h"
#include "TLine.h"
#include "TMath.h"
#include "TROOT.h"

#include <iostream>
#include <string>
#include <vector>

using namespace WireCell;
using namespace std;

int main(int argc, char* argv[])
{
  //std::vector<std::string> geometry;
  //geometry.push_back("/home/xiaoyueli/BNLIF/wire-cell/geom_10kt_workspace_v1.txt");
    DetectorGDS gds;
    
    gds.set_ncryos(1);
    gds.set_napas(0,4);
    Vector center0(-0*units::cm, -300.05*units::cm, 115.318875*units::cm);
    Vector halves0(3.995355*units::cm, 299.5*units::cm, 115.318875*units::cm);
    gds.set_apa(0, 0, 35.71*units::deg, 35.71*units::deg, 0.4667*units::cm, 0.4667*units::cm, 0.479*units::cm, center0, halves0);
    Vector center1(-0*units::cm, 300.05*units::cm, 115.318875*units::cm);
    Vector halves1(3.995355*units::cm, 299.5*units::cm, 115.318875*units::cm);
    gds.set_apa(0, 1, 35.71*units::deg, 35.71*units::deg, 0.4667*units::cm, 0.4667*units::cm, 0.479*units::cm, center1, halves1);
    Vector center2(-0*units::cm, -300.05*units::cm, 347.709*units::cm);
    Vector halves2(3.995355*units::cm, 299.5*units::cm, 115.318875*units::cm);
    gds.set_apa(0, 2, 35.71*units::deg, 35.71*units::deg, 0.4667*units::cm, 0.4667*units::cm, 0.479*units::cm, center2, halves2);
    Vector center3(-0*units::cm, 300.05*units::cm, 347.709*units::cm);
    Vector halves3(3.995355*units::cm, 299.5*units::cm, 115.318875*units::cm);
    gds.set_apa(0, 3, 35.71*units::deg, 35.71*units::deg, 0.4667*units::cm, 0.4667*units::cm, 0.479*units::cm, center3, halves3);
    gds.buildGDS(); 
    
    //gds.read_geometry("/home/xiaoyueli/BNLIF/wire-cell/geom_10kt_workspace_v1.txt");    
    std::cout<<"built detector"<<std::endl;
    gROOT->ProcessLine(".x ~/rootlogon.C");
    
    TCanvas *c = new TCanvas();
    c->Range(-5*units::cm, -610*units::cm, 470*units::cm, 610*units::cm);    
    c->SetFillColorAlpha(0,0);
    TLine *l = new TLine();
    //l->SetLineWidth(0);
    c->cd();

    int nwires[8][3]={0};
    
    int colors[] = {2,4,1};
    for (int iplane=0; iplane<3; ++iplane) {
      //c->Clear();
      for (short cryo = 0; cryo < gds.ncryos(); cryo++) {
	for (short apa = 0; apa < gds.napa(cryo); apa++) {
	  //std::cout<<"before"<<std::endl;
	  const WrappedGDS *apa_gds = gds.get_apaGDS(cryo, apa);
	  //std::cout<<"got apa GDS"<<std::endl;
	  WirePlaneType_t plane = (WirePlaneType_t)iplane;
	  GeomWireSelection wip = apa_gds->wires_in_plane(plane);
	  //std::cout<<"\n[CRYO] "<<cryo<<" [APA] "<<apa<<" [PLANE] "<<iplane
	  //	 <<" has "<< wip.size()<<" wires, wire angle is "<<apa_gds->angle(plane)*180/TMath::Pi()<<std::endl;
	  for (auto wit = wip.begin(); wit != wip.end(); ++wit) {
	    const GeomWire& wire = **wit;
	    //for (int index=0; index<(int)wip.size(); ++index) {
	    //const GeomWire* wire = apa_gds->by_planeindex(plane, index);
	    //if (wire.face() = =0) continue;
	    int ntpc;
	    if (wire.face()==0) ntpc = apa*2;
	    else ntpc = apa*2+1;
	    nwires[ntpc][plane] += 1;
	    const Vector& p1 = wire.point1();
	    const Vector& p2 = wire.point2();
	    std::cout<<wire<<" ("<<p1.x<<","<<p1.y<<","<<p1.z<<") ("<<p2.x<<","<<p2.y<<","<<p2.z<<")\n";
	    l->SetLineColor(colors[iplane]);
	    l->SetLineWidth(0);
	    l->DrawLine(p1.z, p1.y, p2.z, p2.y);
	  }
	}	    
      }
    }
    c->SaveAs(Form("/home/xiaoyueli/BNLIF/wire-cell/nav/test/test_detectorgds_workspace.pdf"));
    for (int i=0; i<8; ++i) {
      std::cout<<"tpc"<<i<<" plane0: "<<nwires[i][0]<<"\n"
	       <<"tpc"<<i<<" plane1: "<<nwires[i][1]<<"\n"
	       <<"tpc"<<i<<" plane2: "<<nwires[i][2]<<std::endl;
    }
    
  
}
