#include "WireCellIface/IConfigurable.h"

#include "WireCellIface/IWireParameters.h"
#include "WireCellIface/IWireGenerator.h"
#include "WireCellIface/IWireProvider.h"
#include "WireCellIface/IWireDatabase.h"

#include "WireCellIface/ICellGenerator.h"
#include "WireCellIface/ICellProvider.h"
#include "WireCellIface/ICellDatabase.h"

#include "WireCellUtil/Testing.h"
#include "WireCellUtil/TimeKeeper.h"

#include "WireCellUtil/NamedFactory.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TMarker.h"



#include <iostream>
#include <vector>

using namespace boost::posix_time;
using namespace WireCell;
using namespace std;

int main(int argc, char* argv[])
{
    TimeKeeper tk("test cells");

    // These are here to force the linker to give us the symbols
    WIRECELL_NAMEDFACTORY_USE(WireParams);
    WIRECELL_NAMEDFACTORY_USE(ParamWires);
//    WIRECELL_NAMEDFACTORY_USE(WireDatabase);
    WIRECELL_NAMEDFACTORY_USE(BoundCells);
//    WIRECELL_NAMEDFACTORY_USE(CellDatabase);

    cout << tk("factories made") << endl;

    // fixme: this C++ dance to wire up the interfaces may eventually
    // be done inside a workflow engine.

    // fixme: this needs to be done by a configuration service
    auto wp_cfg = WireCell::Factory::lookup<IConfigurable>("WireParams");
    Assert(wp_cfg, "Failed to get IConfigurable from default WireParams");
    auto cfg = wp_cfg->default_configuration();
    double pitch_mm = 100.0;
    cfg.put("pitch_mm.u", pitch_mm);
    cfg.put("pitch_mm.v", pitch_mm);
    cfg.put("pitch_mm.w", pitch_mm);
    wp_cfg->configure(cfg);
    cout << tk("Configured WireParams") << endl;

    //cout << configuration_dumps(cfg) << endl;

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
    //Assert(747 == nwires);
    cout << tk("Got ParamWires to local collection") << endl;


    // auto wdb = WireCell::Factory::lookup<IWireDatabase>("WireDatabase");
    // Assert(wdb, "Failed to get IWireDatabase from default WireDatabase");
    // cout << "Got WireDatabase IWireDatabase interface @ " << wdb << endl;
    // wdb->load(wires);

    auto bc_gen = WireCell::Factory::lookup<ICellGenerator>("BoundCells");
    Assert(bc_gen, "Failed to get ICellGenerator from default BoundCells");
    cout << "Got BoundCells ICellGenerator interface @ " << bc_gen << endl;
    bc_gen->generate(pw_pro->wires_begin(), pw_pro->wires_end());
    cout << tk("BoundCells generated") << endl;

    auto bc_pro = WireCell::Factory::lookup<ICellProvider>("BoundCells");
    Assert(bc_pro, "Failed to get ICellProvider from default BoundCells");
    cout << "Got BoundCells ICellProvider interface @ " << bc_pro << endl;

    std::vector<const ICell*> cells(bc_pro->cells_begin(),
				    bc_pro->cells_end() );
    int ncells = cells.size();
    cout << "Got " << ncells << " cells" << endl;
    cout << tk("Got BoundCells to local collection") << endl;
    Assert(ncells, "Got no cells");

    WireCell::BoundingBox boundingbox;
    for (int ind = 0; ind < cells.size(); ++ind) {
	// it would be more efficient to find the BB of the wires, but
	// here we just use the center as a test.  This also means the
	// BB does not contain the corners of the cells at the borders
	// of the planes
	boundingbox(cells[ind]->center());
    }
    const Ray& bbox = boundingbox.bounds;
    cout << tk("Made bounding box") << endl;

    // auto cdb = WireCell::Factory::lookup<ICellDatabase>("CellDatabase");
    // Assert(cdb, "Failed to get ICellDatabase from default CellDatabase");
    // cout << "Got CellDatabase ICellDatabase interface @ " << cdb << endl;
    // cdb->load(bc_pro->cells());

    TApplication* theApp = 0;
    if (argc > 1) {
	theApp = new TApplication ("test_iwireprovider",0,0);
    }
    TCanvas c;
    TMarker m;
    m.SetMarkerSize(1);
    m.SetMarkerStyle(20);
    int colors[] = {2,4,1};
    c.DrawFrame(bbox.first.z(), bbox.first.y(), bbox.second.z(), bbox.second.y());
    cout << tk("Started TCanvas") << endl;

    for (int cind = 0; cind < cells.size(); ++cind) {
	const ICell& cell = *cells[cind];

	TPolyLine *pl = new TPolyLine; // Hi and welcome to Leak City.
	PointVector corners = cell.corners();
	for (int corner_ind=0; corner_ind < corners.size(); ++corner_ind) {
	    const Point& corner = corners[corner_ind];
	    pl->SetPoint(corner_ind, corner.z(), corner.y());
	}
	pl->SetFillColor((cind%10)+1); // try to stay out of the vomit colors
	pl->Draw();
    }
    cout << tk("Canvas drawn") << endl;

    if (theApp) {
	theApp->Run();
    }
    else {			// batch
	c.Print("test_icelldatabase.pdf");
    }


    return 0;
}

