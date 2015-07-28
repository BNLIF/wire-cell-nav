#include "WireCellIface/IWireSelectors.h"
#include "WireCellNav/ParamWires.h"
#include "WireCellNav/WireParams.h"
#include "WireCellUtil/Testing.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TView.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"

#include <boost/range.hpp>

#include <iostream>
#include <cmath>

using namespace WireCell;
using namespace std;

void test1()
{
    WireParams params;
    ParamWires pw;
    pw.generate(params);

    std::vector<const IWire*> wires(pw.wires_begin(), pw.wires_end());

    cerr << "Got " << wires.size() << " wires" <<endl;
    Assert(wires.size());
    int last_plane = -1;
    int last_index = -1;
    for (auto wit = wires.begin(); wit != wires.end(); ++wit) {
	const IWire& wire = **wit;
	int iplane = wire.plane() - kFirstPlane;
	int ident = (1+iplane)*100000 + wire.index();
	Assert(ident == wire.ident());

	if (iplane == last_plane) {
	    ++last_index;
	}
	else {
	    last_plane = iplane;
	    last_index = 0;
	}
	Assert(last_index == wire.index());

    }
}

void test2()
{
    double pitches[] = {10.0, 5.0, 3.0, -1};
    int want[] = {371, 747, 1243, 0};
    for (int ind=0; pitches[ind] > 0.0; ++ind) {
	WireParams params;
	Configuration cfg = params.default_configuration();
	cfg.put("pitch_mm.u", pitches[ind]);
	cfg.put("pitch_mm.v", pitches[ind]);
	cfg.put("pitch_mm.w", pitches[ind]);
	params.configure(cfg);
	ParamWires pw;
	pw.generate(params);
	std::vector<const IWire*> wires(pw.wires_begin(), pw.wires_end());
	int nwires = wires.size();
	cout << ind << ": pitch=" << pitches[ind] << " nwires=" << nwires << " (want=" << want[ind] << ")" << endl;
	Assert(nwires == want[ind], "Wrong number of wires");
	Assert(configuration_dumps(cfg).size(), "Failed to dump cfg");
    }
}

void test3D(bool interactive)
{
    WireParams params;
    ParamWires pw;
    pw.generate(params);

    const Ray& bbox = params.bounds();

    TApplication* theApp = 0;
    if (interactive) {
	theApp = new TApplication ("test_paramwires3d",0,0);
    }

    TCanvas c;

    TView* view = TView::CreateView(1);
    view->SetRange(bbox.first.x(),bbox.first.y(),bbox.first.z(),
		   bbox.second.x(),bbox.second.y(),bbox.second.z());
    view->ShowAxis();
    int colors[3] = {2, 4, 1};


    std::vector<const IWire*> wires(pw.wires_begin(), pw.wires_end());
    Assert(wires.size(), "Got no wires");

    vector<const IWire*> u_wires, v_wires, w_wires;
    copy_if(wires.begin(), wires.end(), back_inserter(u_wires), select_u_wires);
    copy_if(wires.begin(), wires.end(), back_inserter(v_wires), select_v_wires);
    copy_if(wires.begin(), wires.end(), back_inserter(w_wires), select_w_wires);
    size_t n_wires[3] = {
	u_wires.size(),
	v_wires.size(),
	w_wires.size()
    };

    double max_width = 5;
    for (auto wit = wires.begin(); wit != wires.end(); ++wit) {
	const IWire& wire = **wit;
	int iplane = wire.plane();
	int index = wire.index();

	Assert(n_wires[iplane], "Empty plane");
	double width = ((index+1)*max_width)/n_wires[iplane];

	const Ray ray = wire.ray();
	TPolyLine3D* pl = new TPolyLine3D(2);
	pl->SetPoint(0, ray.first.x(), ray.first.y(), ray.first.z());
	pl->SetPoint(1, ray.second.x(), ray.second.y(), ray.second.z());
	pl->SetLineColor(colors[iplane]);
	pl->SetLineWidth(width);
	pl->Draw();
    }

    if (theApp) {
	theApp->Run();
    }
    else {			// batch
	c.Print("test_paramwires3d.pdf");
    }

}

int main(int argc, char** argv)
{
    test1();
    test2();
    test3D(argc>1);
    

    return 0;
}
