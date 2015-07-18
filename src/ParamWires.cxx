#include "WireCellNav/ParamWires.h"
#include "WireCellUtil/Intersection.h"
#include "WireCellUtil/NamedFactory.h"

#include <cmath>
#include <iostream> 		// debugging
#include <vector>

using namespace WireCell;

WIRECELL_NAMEDFACTORY(ParamWires);
WIRECELL_NAMEDFACTORY_ASSOCIATE(ParamWires, IWireProvider);
WIRECELL_NAMEDFACTORY_ASSOCIATE(ParamWires, IWireGenerator);

static int number_of_wires = 0;

class ParamWire : public IWire {
    WirePlaneType_t m_plane;
    int m_index;
    Ray m_ray;

public:
    ParamWire(WirePlaneType_t plane, int index, const Ray& ray)
	: m_plane(plane), m_index(index), m_ray(ray)
    {
	++number_of_wires;
    }
    virtual ~ParamWire()
    {
	--number_of_wires;
    }

    int ident() const {
	int iplane = m_plane - kFirstPlane;
	++iplane;
	return iplane*100000 + m_index;
    }

    WirePlaneType_t plane() const { return m_plane; }
	
    int index() const { return m_index; }

    int channel() const { return ident(); }

    WireCell::Ray ray() const { return m_ray; }

public:		     // just for us lucky functions local to this file
    void set_index(int ind) { m_index = ind; }
    void set_plane(WirePlaneType_t plane) { m_plane = plane; }

};


static ParamWire* make_wire(int index, const Point& point,
			    const Point& proto, const Ray& bounds)
{
    double number = index;
    const Point pt1 = point;
    const Point pt2  = pt1 + proto;
    const Ray wireray(pt1, pt2);

    Ray hits;
    int hitmask = box_intersection(bounds, wireray, hits);
    if (3 != hitmask) {
	return 0;
    }
    return new ParamWire(kUnknownWirePlaneType, index, hits);
}

struct SortByIndex {
    inline bool operator()(const ParamWire* lhs, const ParamWire* rhs) {
	return lhs->index() < rhs->index();
    }
};    


static void make_one_plane(WireSet& store, WirePlaneType_t plane,
			   const Ray& bounds,
			   const Ray& step)
{
    const Vector drift(-1,0,0);
    const Point starting_point = step.first;
    const Vector pitch = step.second - starting_point;
    const Vector proto = drift.cross(pitch).norm();


    std::vector<ParamWire*> all_wires;

    int pos_index = 0;
    Point offset = starting_point;
    while (true) {		// go in positive pitch direction
	ParamWire* wire = make_wire(pos_index, offset, proto, bounds);
	if (! wire) { break; }
	all_wires.push_back(wire);
	offset = wire->center() + pitch;
	pos_index += 1;
    }

    int neg_index = -1;		// now go in negative pitch direction
    offset = all_wires[0]->center();
    const Vector neg_pitch = -1.0 * pitch;
    while (true) {		// go in negative pitch direction
	ParamWire* wire = make_wire(neg_index, offset, proto, bounds);
	if (! wire) { break; }
	all_wires.push_back(wire);
	offset = wire->center() + neg_pitch;
	neg_index -= 1;
    }

    // order by index
    std::sort(all_wires.begin(), all_wires.end(), SortByIndex());

    // load in to store and fix up index and plane
    for (int ind=0; ind<all_wires.size(); ++ind) {
	ParamWire* pwire = all_wires[ind];
	pwire->set_index(ind);
	pwire->set_plane(plane);
	Wire wire(pwire);	// intern
	store.insert(wire);
    }
	
    //std::cerr << "Made "<<store.size()<<" wires for plane " << plane << std::endl;
    //std::cerr << "step = " << step << std::endl;
    //std::cerr << "bounds = " << bounds << std::endl;
}


void ParamWires::generate(const IWireParameters& params)

{
    this->clear();

    make_one_plane(m_wire_store, kUwire, params.bounds(), params.pitchU());
    make_one_plane(m_wire_store, kVwire, params.bounds(), params.pitchV());
    make_one_plane(m_wire_store, kWwire, params.bounds(), params.pitchW());
}

const WireSet& ParamWires::wires() const
{
    return m_wire_store;
}


void ParamWires::clear() {
    m_wire_store.clear();
}

ParamWires::ParamWires()
{
}

ParamWires::~ParamWires()
{
    this->clear();
}


