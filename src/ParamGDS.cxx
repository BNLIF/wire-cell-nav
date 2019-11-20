#include "WCPNav/ParamGDS.h"
#include "WCPData/Intersection.h"
#include "WCPData/Point.h"

#include <iostream>		// debug
using namespace std;
using namespace WCP;


// in the new branch, this is part of IWire
static Vector wire_center(const GeomWire& wire)
{
    double half = 0.5;
    return half*(Vector(wire.point1())+Vector(wire.point2()));
}

struct SortByIndex {
    inline bool operator()(const GeomWire& lhs, const GeomWire& rhs) {
        return lhs.index() < rhs.index();
    }
};    



bool ParamGDS::make_wire(std::vector<GeomWire>& all_wires,
			 int index, const Vector& point,
			 const Vector& proto)
{
    double number = index;

    Vector hit1, hit2;
    int hitmask = box_intersection(minbound, maxbound,
				   point, proto,
				   hit1, hit2);
    if (3 != hitmask) {
        return false;
    }
    
    GeomWire tmp(0, kUnknownWirePlaneType, index, 0, hit1, hit2);
    all_wires.push_back(tmp);
    return true;
}

int ParamGDS::make_wire_plane(const Vector& starting_point, const Vector& pitch,
			      WirePlaneType_t plane)
{
    const Vector drift(-1,0.0);
    const Vector proto = pitch.cross(drift).norm();

    int global_count = this->get_wires().size();

    std::vector<GeomWire> all_wires;

    int pos_index = 0;
    Point offset = starting_point;
    while (true) {              // go in positive pitch direction
	bool ok = make_wire(all_wires, pos_index, offset, proto);
	if (!ok) { break; }
	GeomWire& wire = *all_wires.rbegin();
        offset = wire_center(wire) + pitch;
        pos_index += 1;
    }

    int neg_index = -1;         // now go in negative pitch direction
    const Vector neg_pitch = -1.0 * pitch;
    offset = wire_center(all_wires[0]) + neg_pitch; // start one below first upward going one
    while (true) {              // go in negative pitch direction
	bool ok = make_wire(all_wires, neg_index, offset, proto);
	if (!ok) { break; }
	GeomWire& wire = *all_wires.rbegin();
        offset = wire_center(wire) + neg_pitch;
        neg_index -= 1;
    }

    // order by index
    std::sort(all_wires.begin(), all_wires.end(), SortByIndex());

    // load in to store and fix up index and plane
    int nwires = all_wires.size();
    for (int ind=0; ind<nwires; ++ind) {
        GeomWire& tmp = all_wires[ind];
	int chan = global_count + ind;
	GeomWire gwire(chan, plane, ind, chan, tmp.point1(), tmp.point2());
	this->add_wire(gwire);
    }

    return nwires;
}

ParamGDS::ParamGDS(const Vector& minbound, const Vector& maxbound, 
		   const Vector& offsetU, const Vector& pitchU,
		   const Vector& offsetV, const Vector& pitchV,
		   const Vector& offsetY, const Vector& pitchY)
    : GeomDataSource()
    , minbound(minbound)
    , maxbound(maxbound)
{
    make_wire_plane(offsetU, pitchU, kUwire);
    make_wire_plane(offsetV, pitchV, kVwire);
    make_wire_plane(offsetY, pitchY, kYwire);
}

ParamGDS::~ParamGDS()
{
}

