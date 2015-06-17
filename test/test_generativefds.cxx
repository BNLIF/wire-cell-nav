#include "WireCellNav/ExampleGDS.h"
#include "WireCellNav/GenerativeFDS.h"
#include "WireCellNav/PepperDepositor.h"
#include "WireCellNav/SliceDataSource.h"

#include "WireCellData/Units.h"
#include "WireCellData/Slice.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace WireCell;
int main () {
    using namespace std;
    const float width = 1*units::m;
    const PepperDepositor::MinMax side(-width,width);
    const PepperDepositor::MinMax charge(1,100);
    PepperDepositor dep(side, side, charge);
    
    GeomDataSource* gds = make_example_gds();    

    GenerativeFDS fds(dep, *gds);

    for (int ind=0; ind<10; ++ind) {
	assert (fds.jump(ind) >= 0);
	Frame frame = fds.get();
	assert (frame.index == ind);
	assert (frame.traces.size() > 0);

	cerr << "FRAME["<<ind<<"]" << endl;

	const SimTruthSelection truth = fds.truth();
	assert (truth.size() > 0);

	cerr << "TRUTH["<<ind<<"] has " << truth.size() << " hits" << endl;
	for (int tind=0; tind<truth.size(); ++tind) {
	    cerr << "\tq=" << truth[tind]->charge() << " @ " << truth[tind]->pos() << endl;
	}

	SliceDataSource sds(fds);
	assert (sds.size() > 0);
	while (sds.next() >= 0) {
	    const Slice& slice = sds.get();
	    const Channel::Group& group = slice.group();

	    if (!group.size()) {
		continue;
	    }
	    cerr << "\tSLICE @ " << slice.tbin() << " with " << group.size() << endl;
	    for (int qind=0; qind < group.size(); ++qind) {
		int chid = group[qind].first;
		const GeomWireSelection& wires = gds->by_channel(chid);
		int plane = wires[0]->plane();
		cerr << "\t\tplane="<<plane<<" ch=" << chid << " q=" << group[qind].second << endl;
	    }

	}
	
    }

    return 0;
}
