#include "WireCellNav/ParamWires.h"
#include "WireCellData/Intersection.h"
#include "WireCellData/Point.h"

using namespace WireCell;


bool ParamWires::inside(const Vector& point) const
{
    for (int ind=0; ind<3; ++ind) {
	if (point[ind] < minbound[ind]) return false;
	if (point[ind] > maxbound[ind]) return false;
    }
    return true;
}

int ParamWires::make_wire_plane(const Vector& offset, const Vector& pitch,
			      WirePlaneType_t plane) const
{
    int global_count = this->get_wires().size();
    Vector proto = drift.cross(pitch).norm(); 

    int index = -1;
    while (true) {
	++index;

	double number = index;
	Vector ptonline = offset +number*pitch;
    
	Vector hit1, hit2;
	int hitmask = box_intersection(minbound, maxbound, ptonline, proto, hit1, hit2);    
	if (3 != hitmask) {
	    break;
	}
	int chan = global_count + index;
	
	wires.insert(GeomWire(chan, plane, index, chan, Point(hit1), Point(hit2)));
    }
    return index;
}

ParamWires::ParamWires(const Vector& minbound, const Vector& maxbound, const Vector& drift,
		       const Vector& offsetU, const Vector& pitchU,
		       const Vector& offsetV, const Vector& pitchV,
		       const Vector& offsetY, const Vector& pitchY)
    : minbound(minbound)
    , maxbound(maxbound)
    , drift(drift)
{
    make_wire_plane(offsetU, pitchU, kUwire);
    make_wire_plane(offsetV, pitchV, kVwire);
    make_wire_plane(offsetY, pitchY, kYwire);
}

const WireCell::GeomWireSet& ParamWires::get_wires() const
{
    return wires;
}

ParamWires::~ParamWires()
{
}

