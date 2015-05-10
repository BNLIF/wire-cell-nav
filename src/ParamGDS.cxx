#include "WireCellNav/ParamGDS.h"
#include "WireCellData/Intersection.h"
#include "WireCellData/Point.h"

using namespace WireCell;


bool ParamGDS::inside(const Vector& point)
{
    for (int ind=0; ind<3; ++ind) {
	if (point[ind] < minbound[ind]) return false;
	if (point[ind] > maxbound[ind]) return false;
    }
    return true;
}

int ParamGDS::make_wires(const Vector& start, const Vector& step, 
			 const Vector& proto, WirePlaneType_t plane)
{
    int global_count = this->get_wires().size();

    int index = -1;
    while (true) {
	++index;

	Vector point = start + double(index) * step;
	
	if (!inside(point)) {
	    break;
	}

	Vector hit1, hit2;
	int hitmask = box_intersection(minbound, maxbound, point, proto, hit1, hit2);
	if (hitmask != 3) {
	    std::cerr << "ERROR: failed to find both wire end points "
		      << " hitmask=" << hitmask
		      << " plane#" << plane
		      << " wire#" << index
		      << " @" << point
		      << " hit1=" << hit1
		      << " hit2=" << hit2
		      << std::endl;
	    return 0;
	}

	int chan = global_count + index;
	GeomWire wire(chan, plane, index, chan, Point(hit1), Point(hit2));
	this->add_wire(wire);
    }
    return index;
}
int ParamGDS::make_wire_plane(const Vector& offset, const Vector& pitch,
			      WirePlaneType_t plane)
{
    // start in center of bounds respecting the offset
    Vector center = 0.5*(minbound+maxbound);

    Vector hit1, hit2;
    int hitmask = box_intersection(minbound, maxbound, center, pitch, hit1, hit2);    

    Vector start = hit1;
    Vector stride = center-start;
    if (stride.dot(pitch) < 0) {
	start = hit2;
    }
    start = start + offset;

    // unit vector in direction of wires of this plane
    Vector proto = drift.cross(pitch).norm(); 

    // std::cerr << "Making plane #" << plane 
    // 	      << " proto=" << proto
    // 	      << " start=" << start
    // 	      << " pitch=" << pitch
    // 	      << " offset=" << offset
    // 	      << std::endl;

    return make_wires(start, pitch, proto, plane);
}


ParamGDS::ParamGDS(const Vector& minbound, const Vector& maxbound, const Vector& drift,
		   const Vector& offsetU, const Vector& pitchU,
		   const Vector& offsetV, const Vector& pitchV,
		   const Vector& offsetY, const Vector& pitchY)
    : GeomDataSource()
    , minbound(minbound)
    , maxbound(maxbound)
    , drift(drift)
{
    make_wire_plane(offsetU, pitchU, kUwire);
    make_wire_plane(offsetV, pitchV, kVwire);
    make_wire_plane(offsetY, pitchY, kYwire);
}

ParamGDS::~ParamGDS()
{
}

