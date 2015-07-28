// Note, this test directly links to Nav classes.  You should instead
// use Interfaces in real applications.

#include "WireCellIface/IWireSelectors.h"
#include "WireCellNav/ParamWires.h"
#include "WireCellNav/WireParams.h"
#include "WireCellNav/BoundCells.h"

#include "WireCellUtil/Testing.h"
#include "WireCellUtil/TimeKeeper.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TPolyLine.h"

#include <boost/range.hpp>

#include <iostream>
#include <cmath>

using namespace WireCell;
using namespace std;

int main(int argc, char** argv)
{
    bool interactive = argc > 1;

    TimeKeeper tk("test bound cells");

    WireParams params;

#if 0
    double pitch = 10.0;
    auto cfg = params.default_configuration();
    cfg.put("pitch_mm.u", pitch);
    cfg.put("pitch_mm.v", pitch);
    cfg.put("pitch_mm.w", pitch);
    params.configure(cfg);
#endif

    ParamWires pw;
    pw.generate(params);
    cout << tk("generated wires") << endl;

    BoundCells bc;
    bc.generate(pw.wires_begin(), pw.wires_end());

    cout << tk("generated cells") << endl;

    vector<const ICell*> cells(bc.cells_begin(), bc.cells_end());
    cout << "Got " << cells.size() << " cells" << endl;
    cout << tk("in this time") << endl;

    const Ray& bbox = params.bounds();

    TApplication* theApp = 0;
    if (interactive) {
	theApp = new TApplication ("test_boundcells",0,0);
    }

    TCanvas c;
    c.DrawFrame(bbox.first.z(), bbox.first.y(),
		bbox.second.z(), bbox.second.y());

    const int ncolors= 4;
    int colors[ncolors] = {2, 4, 6, 8};


    int ncells = cells.size();
    for (int cind = 0; cind < ncells; ++cind) {
	const ICell& cell = *cells[cind];

        WireCell::PointVector corners = cell.corners();
	const Point center = cell.center();
	//cerr << cind << ": " << center<< endl;

	size_t ncorners = corners.size();
	
	TPolyLine* pl = new TPolyLine;
	int ind=0;
	for (; ind<ncorners; ++ind) {
	    const Point& p = corners[ind];
	    pl->SetPoint(ind, p.z(), p.y());
	}
	pl->SetPoint(ind, corners[0].z(), corners[0].y());
	pl->SetLineColor(colors[cind%ncolors]);
	pl->Draw();
    }

    if (theApp) {
	theApp->Run();
    }
    else {			// batch
	c.Print("test_boundcells.pdf");
    }


    return 0;
}
