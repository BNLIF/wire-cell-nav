#include "WireCellNav/ParamWires.h"
#include "WireCellUtil/Testing.h"

#include <iostream>
#include <cmath>

using namespace WireCell;
using namespace std;

void test1()
{
    ParamWires pw;
    pw.generate();
    const WireStore& wires = pw.wires();

    cerr << "Got " << wires.size() << " wires" <<endl;
    Assert(wires.size());
    int last_plane = -1;
    int last_index = -1;
    for (auto wit=wires.begin(); wit != wires.end(); ++wit) {
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
    int want[] = {331, 663, 1103, 0};
    for (int ind=0; pitches[ind] > 0.0; ++ind) {
	ParamWires pw;
	Configuration cfg = pw.default_configuration();
	cfg.put("pitch_mm.u", pitches[ind]);
	cfg.put("pitch_mm.v", pitches[ind]);
	cfg.put("pitch_mm.w", pitches[ind]);
	pw.configure(cfg);
	int nwires = pw.wires().size();
	cout << ind << ": pitch=" << pitches[ind] << " nwires=" << nwires << " (want=" << want[ind] << ")" << endl;
	Assert(nwires == want[ind]);
	Assert(configuration_dumps(cfg).size());
    }
}

int main()
{
    test1();
    test2();
    return 0;
}
