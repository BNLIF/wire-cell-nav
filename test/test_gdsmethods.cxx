#include "WireCellNav/ExampleGDS.h"

#include <iostream>
#include <string>

using namespace WireCell;
using namespace std;

template<typename OK>
void assert(const OK& ok, const std::string& msg = "")
{
    if (ok) return;

    cerr << "FAIL: " << msg << endl;
    exit(1);
}

int main()
{
    GeomDataSource* pgds = make_example_gds();
    assert(pgds, "Can not make example gds");
    GeomDataSource& gds = *pgds;

    if (!gds.get_wires().size()) {
	return 2;
    }

    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t plane = (WirePlaneType_t)iplane;
	GeomWireSelection wip = gds.wires_in_plane(plane);
	double pitch = gds.pitch(plane);
	double last_distance = -99999;

	Vector vpitch = gds.pitch_unit_vector(plane);

	for (int iaxis=0; iaxis<3; ++iaxis) {
	    std::pair<float, float> mm = gds.minmax(iaxis, plane);
	    cerr << "axis " << iaxis << ": " << mm.first << " " << mm.second << endl;
	}

	for (int wind=0; wind<wip.size(); ++wind) {
	    cerr << "\nplane #" << iplane << " wire #" << wind << endl;


	    const GeomWire* wire = wip[wind];
	    assert(wire, "no wire");

	    double wire_dist = gds.wire_dist(*wire);

	    cerr << "WIRE:" << *wire
		 << " " << wire->point1() << " --> " << wire->point2()
		 << " [" << wire_dist << "]"
		 << endl;


	    assert(wire_dist > last_distance);
	    last_distance = wire_dist;

	    Vector center = 0.5*Vector(wire->point1() + wire->point2());
	    double center_dist = gds.wire_dist(center);
	    cerr << "center=" << center << " dist=" << center_dist << endl;
	    assert(gds.contained_yz(center), "center not yz-contained");
	    assert(gds.contained(center), "center not contained");

	    const GeomWire* wclosest = gds.closest(center, plane);
	    cerr << "CLOSEST:" << *wclosest
		 << " " << wclosest->point1() << " --> " << wclosest->point2()
		 << endl;
	    assert (wclosest && wclosest == wire, "wire isn't own closest wire");

	    Vector Pbelow = center - vpitch*(0.1*pitch);
	    const GeomWire* wbelow = gds.closest(Pbelow, plane);
	    assert (wbelow && wbelow == wire, "just below wire isn't own closest wire");
	    GeomWirePair bounds_below = gds.bounds(Pbelow, plane);
	    assert (bounds_below.second == wire, "just below not bounded above");
	    

	    Vector Pabove = center + vpitch*(0.1*pitch);
	    const GeomWire* wabove = gds.closest(Pabove, plane);
	    assert (wabove && wabove == wire, "just above wire isn't own closest wire");
	    GeomWirePair bounds_above = gds.bounds(Pabove, plane);
	    assert (bounds_above.first == wire, "just above not bounded below");


	}
    
    }
    return 0;
}
