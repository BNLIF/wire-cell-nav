#include "WireCellNav/ParamWires.h"
#include "WireCellNav/WireParams.h"
#include "WireCellUtil/Testing.h"

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

int main()
{
    test1();
    test2();
    return 0;
}
