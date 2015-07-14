#include "WireCellNav/WireDatabase.h"
#include "WireCellNav/ParamWires.h"
#include "WireCellUtil/Testing.h"

#include <iostream>
#include <string>

using namespace WireCell;
using namespace std;

void dump(IWireDatabase& wdb, string name, const IWire& wire)
{
    Vector center = wire.center();
    double wire_dist = wdb.wire_dist(wire);
    double center_dist = wdb.wire_dist(center, wire.plane());
    cerr
	<< "(" << (void*) &wire << ") "
	<< name << ":\t"
	<< wire.plane() << "/" << wire.index() << "/" << wire.ident() 
	<< " [" << wire_dist << "]"
	<< " " << wire.ray() << " @ " << center 
	<< endl;
    Assert(fabs(wire_dist - center_dist) < 1e-12);
}

int main()
{
    ParamWires pw;
    pw.generate();
    WireDatabase wdb;
    wdb.load(pw.wires());

    Assert(wdb.wires_in_plane().size());

    for (int iplane=0; iplane < 3; ++iplane) {
	WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane);

	const WireVector& wip = wdb.wires_in_plane(plane);
	Assert(wip.size());

	const WireVector& wip_other = wdb.wires_in_plane(plane);
	Assert(&wip == &wip_other); // they better be cached

	double pitch = wdb.pitch(plane);
	Assert(pitch > 0);
	double last_distance = -99999;

	Vector vpitch = wdb.pitch_unit_vector(plane);
	Assert(vpitch.magnitude() == 1.0);

	Ray bbox = wdb.bounding_box();
	cerr << "Bounding box: " << bbox << endl;
	for (int iaxis=0; iaxis<3; ++iaxis) {
	    Assert(bbox.first[iaxis] < bbox.second[iaxis]);
	}

	for (int wind=0; wind<wip.size(); ++wind) {
	    cerr << "\nplane #" << iplane << " wire #" << wind << endl;

	    const IWire* wire = wip[wind];
	    Assert(wire, "no wire");

	    double wire_dist = wdb.wire_dist(*wire);
	    dump(wdb, "CURRENT", *wire);

	    Assert(wire_dist > last_distance);
	    last_distance = wire_dist;

	    Vector center = wire->center();
	    double center_dist = wdb.wire_dist(center, plane);
	    Assert(point_contained(center, bbox), "center not contained");

	    cerr << "center: [" << center_dist << "] " <<  center << endl;

	    WirePair wbound = wdb.bounding_wires(center, plane);
	    if (wbound.first) {
		dump (wdb, "BELOW", *wbound.first);
	    }
	    if (wbound.second) {
		dump (wdb, "ABOVE", *wbound.second);
	    }


	    const IWire* wclosest = wdb.closest(center, plane);
	    dump(wdb, "CLOSEST", *wclosest);
	    Assert (wclosest && wclosest == wire, "wire isn't own closest wire");

	    Vector Pbelow = center - vpitch*(0.1*pitch);
	    const IWire* wbelow = wdb.closest(Pbelow, plane);
	    Assert (wbelow && wbelow == wire, "just below wire isn't own closest wire");
	    WirePair bounds_below = wdb.bounding_wires(Pbelow, plane);
	    Assert (bounds_below.second == wire, "just below not bounded above");
	    
	    Vector Pabove = center + vpitch*(0.1*pitch);
	    const IWire* wabove = wdb.closest(Pabove, plane);
	    Assert (wabove && wabove == wire, "just above wire isn't own closest wire");
	    WirePair bounds_above = wdb.bounding_wires(Pabove, plane);
	    Assert (bounds_above.first == wire, "just above not bounded below");
	}
    
    }
    return 0;
}
