#include "WireCellNav/ExampleGDS.h"
#include "WireCellNav/GenerativeFDS.h"
#include "WireCellNav/PepperDepositor.h"
#include "WireCellNav/SliceDataSource.h"

#include "WireCellData/Units.h"
#include "WireCellData/Slice.h"

#include <iostream>
#include <string>

using namespace std;
using namespace WireCell;


template<typename OK>
void assert(const OK& ok, string msg="error")
{
    if (ok) { return; }
    cerr << msg << endl;
    exit(-1);
}


int main () {
    using namespace std;
    const float width = 0.5*units::m;
    const PepperDepositor::MinMax drift_dim(0, 2.0*width), trans_dim(-width,width);
    const PepperDepositor::MinMax charge(1,100);
    PepperDepositor dep(drift_dim, trans_dim, trans_dim, charge, 5);
    
    GeomDataSource* gds = make_example_gds(10*units::mm);

    GenerativeFDS fds(dep, *gds);

    for (int ind=0; ind<10; ++ind) {
	assert (fds.jump(ind) >= 0, "error: jump fds");
	Frame frame = fds.get();
	assert (frame.index == ind, "error: frame index");
	assert (frame.traces.size() > 0, "error: no traces");

	cerr << "FRAME["<<ind<<"]" << endl;

	const SimTruthSelection truth = fds.truth();
	assert (truth.size() > 0, "error: gov't detected");

	cerr << "TRUTH["<<ind<<"] has " << truth.size() << " hits" << endl;
	for (int tind=0; tind<truth.size(); ++tind) {
	    cerr << "\t# " << tind 
		 << ": q=" << truth[tind]->charge() << " @ " << truth[tind]->pos() << endl;
	}

	SliceDataSource sds(fds);
	assert (sds.size() > 0, "error: no slices");
	while (sds.next() >= 0) {
	    const Slice& slice = sds.get();
	    const Channel::Group& group = slice.group();

	    if (!group.size()) {
		continue;
	    }
	    int wire_hits = 0;
	    float total_charge=0.0, plane_charge[3] = {0.0};
	    int chargeless_hits[3] = {0};
	    for (int qind=0; qind < group.size(); ++qind) {
		int chid = group[qind].first;
		const GeomWireSelection& wires = gds->by_channel(chid);
		int plane = wires[0]->plane();
		float charge = group[qind].second;
		if (charge <= 0.0) {
		    chargeless_hits[plane] += 1;
		    continue;
		}
		cerr << "\t\tgroup#" << qind << "/" << group.size()
		     << " q=" << charge
		     << " p=" << plane
		     << " ch=" << chid
		     << endl;
		plane_charge[plane] += charge;
		total_charge += charge;
		++wire_hits;
	    }

	    if (total_charge>0) {
		cerr << "\tSLICE with charge @ tbin=" << slice.tbin()
		     << " q/p=" << total_charge/3.0 << " in " << wire_hits << " wire hits" << endl;
		for (int pind=0; pind<3; ++pind) {
		    cerr << "\t\t plane=" << pind
			 << " q=" << plane_charge[pind]
			 << " #empty=" << chargeless_hits[pind]
			 << endl;
		}
	    }
	    else {
		//cerr << "\tSLICE with no charge @ tbin=" << slice.tbin() << endl;
	    }
	}
	
    }

    return 0;
}
