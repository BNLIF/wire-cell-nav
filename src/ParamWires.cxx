#include "WireCellNav/ParamWires.h"
#include "WireCellUtil/Intersection.h"

#include <iostream>		// debugging
using namespace WireCell;

class ParamWire : public IWire {
    WirePlaneType_t m_plane;
    int m_index;
    Ray m_ray;

public:
    ParamWire(WirePlaneType_t plane, int index, const Ray& ray)
	: m_plane(plane), m_index(index), m_ray(ray)
    {}

    int ident() const {
	int iplane = m_plane - kFirstPlane;
	++iplane;
	return iplane*100000 + m_index;
    }

    WirePlaneType_t plane() const { return m_plane; }
	
    int index() const { return m_index; }

    int channel() const { return ident(); }

    WireCell::Ray ray() const { return m_ray; }

};

static void make_one_plane(WireStore& store, WirePlaneType_t plane,
			   const Ray& bounds,
			   const Ray& step,
			   const Vector& drift)
{

    int global_count = store.size();
    Point offset = step.first;
    Vector pitch = step.second - offset;
    Vector proto = drift.cross(pitch).norm();

    int index = -1;
    while (true) {
	++index;

	double number = index;
	Point pt1 = offset + number*pitch;
	Point pt2  = pt1 + proto;
	Ray wireray(pt1, pt2);

	Ray hits;
	int hitmask = box_intersection(bounds, wireray, hits);
	if (3 != hitmask) {
	    std::cerr << "plane"<<plane<<" hitmask=" << hitmask
		      << " ray=" << wireray
		      << " " << hits << std::endl;
	    break;
	}

	ParamWire* wire = new ParamWire(plane, index, hits);
	std::pair<WireStore::iterator, bool> got = store.insert(wire);
	if (not got.second) {
	    delete wire;
	}
	    
    }
	
    std::cerr << "Made "<<index+1<<" wires for plane " << plane << std::endl;
    std::cerr << "step = " << step << std::endl;
    std::cerr << "bounds = " << bounds << std::endl;
}

ParamWires::ParamWires(const Ray& bounds, 
		       const Ray& stepU, const Ray& stepV, const Ray& stepW, 
		       const Vector& drift)
{
    make_one_plane(m_wire_store, kUwire, bounds, stepU, drift);
    make_one_plane(m_wire_store, kVwire, bounds, stepV, drift);
    make_one_plane(m_wire_store, kWwire, bounds, stepW, drift);
}

const WireStore& ParamWires::wires() const
{
    return m_wire_store;
}


ParamWires::~ParamWires()
{
}



IWireProvider* WireCell::make_paramwires(float dx, float dy, float dz,
					 float pitch, float angle)
{
    const float nudge = 0.5*pitch;

    const Ray bounds(Point(-0.5*dx, -0.5*dy, -0.5*dz),
		     Point(+0.5*dx, +0.5*dy, +0.5*dz));

    //              drift,       Y(up/down),  Z(left/right)
    const Vector oU(0.2*dx, -0.5*dy + nudge, -0.5*dz + nudge);
    const Vector oV(0.3*dx, -0.5*dy + nudge, +0.5*dz - nudge);
    const Vector oW(0.1*dx,             0.0, -0.5*dz + nudge);

    const Vector pU(0, pitch*std::cos(+angle), pitch*std::sin(+angle));
    const Ray stepU(oU, oU+pU);


    const Vector pV(0, pitch*std::cos(-angle), pitch*std::sin(-angle));
    const Ray stepV(oV, oV+pV);

    const Vector pW(0, 0, pitch);
    const Ray stepW(oW, oW+pW);

    return new ParamWires(bounds, stepU, stepV, stepW);
}
