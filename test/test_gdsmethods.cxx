#include "WCPNav/ExampleGDS.h"

#include <iostream>
#include <string>

using namespace WCP;
using namespace std;

template<typename OK>
void assert(const OK& ok, const std::string& msg = "")
{
    if (ok) return;

    cerr << "FAIL: " << msg << endl;
    exit(1);
}

void dump(GeomDataSource& gds, string name, const GeomWire& wire)
{
    Vector center = 0.5*Vector(wire.point1() + wire.point2());
    double wire_dist = gds.wire_dist(wire);
    double center_dist = gds.wire_dist(center, wire.plane());
    cerr
	<< "(" << (void*) &wire << ") "
	<< name << ":\t"
	<< wire.plane() << "/" << wire.index() << "/" << wire.ident() 
	<< " [" << wire_dist << "]"
	<< " " << wire.point1() << " -> " << center << " -> " << wire.point2()
	<< endl;
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
	const GeomWireSelection& wip = gds.wires_in_plane(plane);
	const GeomWireSelection& wip_other = gds.wires_in_plane(plane);

	assert(&wip == &wip_other); // they better be cached

	double pitch = gds.pitch(plane);
	double last_distance = -99999;

	Vector vpitch = gds.pitch_unit_vector(plane);

	for (int iaxis=0; iaxis<3; ++iaxis) {
	    std::pair<float, float> mm = gds.minmax(iaxis, plane);
	    cerr << "axis " << iaxis << ": " << mm.first << " " << mm.second << endl;
	}

	for (int wind=0; wind<wip.size(); ++wind) {
	    cerr << "\nplane #" << iplane << " wire #" << wind
		 << " pitch=" << pitch << " " << vpitch
		 << endl;


	    const GeomWire* wire = wip[wind];
	    assert(wire, "no wire");

	    double wire_dist = gds.wire_dist(*wire);

	    dump(gds, "CURRENT", *wire);

	    assert(wire_dist > last_distance);
	    last_distance = wire_dist;

	    Vector center = 0.5*Vector(wire->point1() + wire->point2());
	    double center_dist = gds.wire_dist(center, plane);
	    assert(gds.contained_yz(center), "center not yz-contained");
	    assert(gds.contained(center), "center not contained");

	    cerr << "center: [" << center_dist << "] " <<  center << endl;

	    const GeomWire* wclosest = gds.closest(center, plane);
	    dump(gds, "CLOSEST", *wclosest);
	    assert (wclosest && wclosest == wire, "wire isn't own closest wire");

	    Vector Pbelow = center - vpitch*(0.1*pitch);
	    double below_dist = gds.wire_dist(Pbelow, plane);
	    cerr << "below: [" << below_dist << "] " <<  Pbelow << endl;

	    const GeomWire* wbelow = gds.closest(Pbelow, plane);
	    assert (wbelow && wbelow == wire, "just below wire isn't own closest wire");
	    GeomWirePair bounds_below = gds.bounds(Pbelow, plane);
	    if (bounds_below.first) {
	    dump(gds, "BELOW 1st", *bounds_below.first);
	    }
	    if (bounds_below.second) {
		dump(gds, "BELOW 2nd", *bounds_below.second);
	    }
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
