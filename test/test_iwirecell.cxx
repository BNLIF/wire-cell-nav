#include "WireCellIface/IConfigurable.h"

#include "WireCellIface/IWireParameters.h"
#include "WireCellIface/IWireGenerator.h"

#include "WireCellIface/ICell.h"
#include "WireCellIface/ITiling.h"

#include "WireCellUtil/BoundingBox.h"
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
    WIRECELL_NAMEDFACTORY_USE(BoundCells);

    cout << tk("factories made") << endl;

    // fixme: this C++ dance to wire up the interfaces may eventually
    // be done inside a workflow engine.

    // fixme: this needs to be done by a configuration service
    auto wp_cfg = WireCell::Factory::lookup<IConfigurable>("WireParams");
    AssertMsg(wp_cfg, "Failed to get IConfigurable from default WireParams");
    auto cfg = wp_cfg->default_configuration();
    double pitch_mm = 100.0;
    cfg.put("pitch_mm.u", pitch_mm);
    cfg.put("pitch_mm.v", pitch_mm);
    cfg.put("pitch_mm.w", pitch_mm);
    wp_cfg->configure(cfg);
    cout << tk("Configured WireParams") << endl;

    //cout << configuration_dumps(cfg) << endl;

    auto wp_wps = WireCell::Factory::lookup<IWireParameters>("WireParams");
    AssertMsg(wp_wps, "Failed to get IWireParameters from default WireParams");
    cout << "Got WireParams IWireParameters interface @ " << wp_wps << endl;
    
    auto pw_gen = WireCell::Factory::lookup<IWireGenerator>("ParamWires");
    AssertMsg(pw_gen, "Failed to get IWireGenerator from default ParamWires");
    cout << "Got ParamWires IWireGenerator interface @ " << pw_gen << endl;
    pw_gen->generate(wp_wps);

    auto pw_seq = WireCell::Factory::lookup<IWireSequence>("ParamWires");
    AssertMsg(pw_seq, "Failed to get IWireSequence from default ParamWires");
    cout << "Got ParamWires IWireSequence interface @ " << pw_seq << endl;

    std::vector<IWire::pointer> wires(pw_seq->wires_begin(), pw_seq->wires_end());
    int nwires = wires.size();
    cout << "Got " << nwires << " wires" << endl;
    //Assert(747 == nwires);
    cout << tk("Got ParamWires to local collection") << endl;


    auto bc_sink = WireCell::Factory::lookup<IWireSink>("BoundCells");
    AssertMsg(bc_sink, "Failed to get IWireSink from default BoundCells");
    cout << "Got BoundCells IWireSink interface @ " << bc_sink << endl;
    bc_sink->sink(pw_seq->wires_range());
    cout << tk("BoundCells generated") << endl;

    auto bc_seq = WireCell::Factory::lookup<ICellSequence>("BoundCells");
    AssertMsg(bc_seq, "Failed to get ICellSequence from default BoundCells");
    cout << "Got BoundCells ICellSequence interface @ " << bc_seq << endl;

    std::vector<ICell::pointer> cells(bc_seq->cells_begin(),
				    bc_seq->cells_end() );
    int ncells = cells.size();
    cout << "Got " << ncells << " cells" << endl;
    cout << tk("Got BoundCells to local collection") << endl;
    AssertMsg(ncells, "Got no cells");

    WireCell::BoundingBox boundingbox;
    for (int ind = 0; ind < cells.size(); ++ind) {
	// it would be more efficient to find the BB of the wires, but
	// here we just use the center as a test.  This also means the
	// BB does not contain the corners of the cells at the borders
	// of the planes
	boundingbox(cells[ind]->center());
    }
    const Ray& bbox = boundingbox.bounds();
    cout << tk("Made bounding box") << endl;

    TApplication* theApp = 0;
    if (argc > 1) {
	theApp = new TApplication ("test_iwirecell",0,0);
    }
    TCanvas c;
    TMarker m;
    m.SetMarkerSize(1);
    m.SetMarkerStyle(20);
    int colors[] = {2,4,1};
    c.DrawFrame(bbox.first.z(), bbox.first.y(), bbox.second.z(), bbox.second.y());
    cout << tk("Started TCanvas") << endl;

    for (int cind = 0; cind < cells.size(); ++cind) {
	ICell::pointer cell = cells[cind];

	TPolyLine *pl = new TPolyLine; // Hi and welcome to Leak City.
	PointVector corners = cell->corners();
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
	c.Print("test_iwirecell.pdf");
    }


    return 0;
}

