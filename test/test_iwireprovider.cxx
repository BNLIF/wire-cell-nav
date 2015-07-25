#include "WireCellIface/IConfigurable.h"
#include "WireCellIface/IWireProvider.h"
#include "WireCellIface/IWireParameters.h"
#include "WireCellIface/IWireGenerator.h"
#include "WireCellIface/IWireDatabase.h"

#include "WireCellUtil/Testing.h"
#include "WireCellUtil/Point.h"

#include "WireCellUtil/NamedFactory.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TMarker.h"

#include <iostream>

using namespace WireCell;
using namespace std;


int main(int argc, char* argv[])
{
    // These are here to force the linker to give us the symbols
    WIRECELL_NAMEDFACTORY_USE(WireParams);
    WIRECELL_NAMEDFACTORY_USE(ParamWires);
//    WIRECELL_NAMEDFACTORY_USE(WireDatabase);

    // fixme: this C++ dance to wire up the interfaces may eventually
    // be done inside a workflow engine.

    // fixme: this needs to be done by a configuration service
    auto wp_cfg = WireCell::Factory::lookup<IConfigurable>("WireParams");
    Assert(wp_cfg, "Failed to get IConfigurable from default WireParams");

    auto cfg = wp_cfg->default_configuration();
    double pitch = 10.0;
    cfg.put("pitch_mm.u", pitch);
    cfg.put("pitch_mm.v", pitch);
    cfg.put("pitch_mm.w", pitch);
    wp_cfg->configure(cfg);
    cout << "Got WireParams IConfigurable interface @ " << wp_cfg << endl;

    auto wp_wps = WireCell::Factory::lookup<IWireParameters>("WireParams");
    Assert(wp_wps, "Failed to get IWireParameters from default WireParams");
    cout << "Got WireParams IWireParameters interface @ " << wp_wps << endl;
    
    auto pw_gen = WireCell::Factory::lookup<IWireGenerator>("ParamWires");
    Assert(pw_gen, "Failed to get IWireGenerator from default ParamWires");
    cout << "Got ParamWires IWireGenerator interface @ " << pw_gen << endl;
    pw_gen->generate(*wp_wps);

    auto pw_pro = WireCell::Factory::lookup<IWireProvider>("ParamWires");
    Assert(pw_pro, "Failed to get IWireProvider from default ParamWires");
    cout << "Got ParamWires IWireProvider interface @ " << pw_pro << endl;

    std::vector<const IWire*> wires(pw_pro->wires_begin(), pw_pro->wires_end());
    int nwires = wires.size();
    cout << "Got " << nwires << " wires" << endl;
    //Assert(1103 == nwires);


    // auto wdb = WireCell::Factory::lookup<IWireDatabase>("WireDatabase");
    // Assert(wdb, "Failed to get IWireDatabase from default WireDatabase");
    // cout << "Got WireDatabase IWireDatabase interface @ " << wdb << endl;
    // wdb->load(wires);

    // Ray bbox = wdb->bounding_box();

    WireCell::BoundingBox boundingbox;
    for (int ind = 0; ind < wires.size(); ++ind) {
	boundingbox(wires[ind]->ray());
    }
    const Ray& bbox = boundingbox.bounds;

    TApplication* theApp = 0;
    if (argc > 1) {
	theApp = new TApplication ("test_iwireprovider",0,0);
    }

    TCanvas c;
    TLine l;
    TMarker m;
    m.SetMarkerSize(1);
    m.SetMarkerStyle(20);
    int colors[] = {2,4,1};

    c.DrawFrame(bbox.first.z(), bbox.first.y(), bbox.second.z(), bbox.second.y());
    for (auto wit = wires.begin(); wit != wires.end(); ++wit) {
	const IWire& wire = **wit;

	Ray wray = wire.ray();

	int iplane = wire.plane();

	l.SetLineColor(colors[iplane]);
	l.DrawLine(wray.first.z(), wray.first.y(), wray.second.z(), wray.second.y());
	Point cent = wire.center();
	m.SetMarkerColor(colors[iplane]);
	m.DrawMarker(cent.z(), cent.y());
    }
    if (theApp) {
	theApp->Run();
    }
    else {			// batch
	c.Print("test_iwireprovider.pdf");
    }

    return 0;
}

