#include "WireCellNav/ExampleGDS.h"

#include <iostream>
#include <cassert>

using namespace WireCell;
using namespace std;

int main()
{
    GeomDataSource* pgds = make_example_gds();
    if (!pgds) {
	return 1;
    }
    GeomDataSource& gds = *pgds;
    if (!gds.get_wires().size()) {
	return 2;
    }

    int ntot = gds.get_wires().size();
    cerr << "#wires = " << ntot << endl;
    assert(ntot == 1243);

    int want_nwires[] = {455, 455, 333};

    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t plane = (WirePlaneType_t)iplane;
	GeomWireSelection wip = gds.wires_in_plane(plane);
	for (int iaxis=0; iaxis<3; ++iaxis) {
	    std::pair<float, float> mm = gds.minmax(iaxis, plane);
	    cerr << "axis " << iaxis << ": " << mm.first << " " << mm.second << endl;
	}

	cerr << "plane #" << iplane << " #wires=" << wip.size() << endl;
	assert(want_nwires[iplane] == wip.size());
	// for (int wind=0; wind<wip.size(); ++wind) {
	//     const GeomWire* wire = wip[wind];
	//     cerr << "plane #" << iplane << " wire #" << wind
	// 	 << " " << wire->point1() << " --> " << wire->point2()
	// 	 << endl;
	// }
    }


    return 0;
}
