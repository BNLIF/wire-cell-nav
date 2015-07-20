#include "WireCellNav/WireDatabase.h"
#include "WireCellNav/WireParams.h"
#include "WireCellNav/ParamWires.h"
#include "WireCellUtil/Testing.h"

#include <iostream>
#include <string>

using namespace WireCell;
using namespace std;

void dump(IWireDatabase& wdb, string name, Wire wire)
{
    Vector center = wire->center();
    double wire_dist = wdb.wire_dist(wire);
    double center_dist = wdb.wire_dist(center, wire->plane());
    double wire_center_dist = wire_dist - center_dist;
    cerr
	<< "(" << (void*) wire.get() << ") "
	<< name << ":\t"
	<< wire->plane() << "/" << wire->index() << "/" << wire->ident() 
	<< " [wd=" << wire_dist << "]-[cd=" << center_dist << "]=" << wire_center_dist << " "
	<< " " << wire->ray() << " @ " << center 
	<< endl;
    Assert(fabs(wire_center_dist) < 1e-11, "wire dist and center dist discrepant");
}

int main()
{
    WireParams params;
    ParamWires pw;
    pw.generate(params);
    WireDatabase wdb;
    wdb.load(pw.wires());

    Assert(wdb.wires_in_plane().size(), "No wires in plane");



    for (int iplane=0; iplane < 3; ++iplane) {
	WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane);

	const WireVector& wip = wdb.wires_in_plane(plane);
	Assert(wip.size(), "No wires in plane");

	const WireVector& wip_other = wdb.wires_in_plane(plane);
	Assert(&wip == &wip_other, "Cache failed"); // they better be cached

	double pitch = wdb.pitch(plane);
	Assert(pitch > 0, "Non-positive pitch");
	double last_distance = -99999;

	Vector vpitch = wdb.pitch_unit_vector(plane);
	Assert(vpitch.magnitude() == 1.0, "Non-unit pitch vector");

	Ray bbox = wdb.bounding_box();
	cerr << "Bounding box: " << bbox << endl;
	for (int iaxis=0; iaxis<3; ++iaxis) {
	    Assert(bbox.first[iaxis] < bbox.second[iaxis], "Misordered bounding box");
	}

	for (int wind=0; wind<wip.size(); ++wind) {
	    cerr << "\nplane #" << iplane << " wire #" << wind << endl;

	    Wire wire = wip[wind];
	    Assert(wire, "no wire");

	    double wire_dist = wdb.wire_dist(wire);
	    dump(wdb, "CURRENT", wire);

	    Assert(wire_dist > last_distance, "Wire distance did not grow");
	    last_distance = wire_dist;

	    Vector center = wire->center();
	    double center_dist = wdb.wire_dist(center, plane);
	    Assert(point_contained(center, bbox), "center not contained");

	    WirePair wbound = wdb.bounding_wires(center, plane);
	    if (wbound.first) {
		dump (wdb, "BELOW", wbound.first);
	    }
	    if (wbound.second) {
		dump (wdb, "ABOVE", wbound.second);
	    }


	    Wire wclosest = wdb.closest(center, plane);
	    dump(wdb, "CLOSEST", wclosest);
	    Assert (wclosest && wclosest == wire, "wire isn't own closest wire");

	    Vector Pbelow = center - vpitch*(0.1*pitch);
	    Wire wbelow = wdb.closest(Pbelow, plane);
	    Assert (wbelow && wbelow == wire, "just below wire isn't own closest wire");
	    WirePair bounds_below = wdb.bounding_wires(Pbelow, plane);
	    Assert (bounds_below.second == wire, "just below not bounded above");
	    
	    Vector Pabove = center + vpitch*(0.1*pitch);
	    Wire wabove = wdb.closest(Pabove, plane);
	    Assert (wabove && wabove == wire, "just above wire isn't own closest wire");
	    WirePair bounds_above = wdb.bounding_wires(Pabove, plane);
	    Assert (bounds_above.first == wire, "just above not bounded below");
	}
    
    }
    return 0;
}
