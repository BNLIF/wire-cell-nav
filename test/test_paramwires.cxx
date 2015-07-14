#include "WireCellNav/ParamWires.h"
#include "WireCellUtil/Testing.h"

#include <iostream>
#include <cmath>

using namespace WireCell;
using namespace std;

int main()
{
    ParamWires pw;
    pw.generate();
    const WireStore& wires = pw.wires();

    cerr << "Got " << wires.size() << " wires" <<endl;
    Assert(wires.size());
    for (auto wit=wires.begin(); wit != wires.end(); ++wit) {
	const IWire& wire = **wit;
	int iplane = wire.plane() - kFirstPlane;
	int ident = (1+iplane)*100000 + wire.index();
	cerr << wire
	     << " ident=" << wire.ident()
	     << " and " << ident
	     << " plane=" << wire.plane()
	     << " and " << iplane
	     << " index=" << wire.index()
	     << " channel=" << wire.channel()
	     << endl;
	Assert(ident == wire.ident());
    }

    return 0;
}
