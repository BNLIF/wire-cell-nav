#include "WireCellNav/ParamWires.h"
#include "WireCellUtil/Intersection.h"
#include "WireCellUtil/NamedFactory.h"

#include <cmath>
#include <iostream> 		// debugging
#include <vector>

using namespace WireCell;

WIRECELL_NAMEDFACTORY(ParamWires);
WIRECELL_NAMEDFACTORY_ASSOCIATE(ParamWires, IWireProvider);
WIRECELL_NAMEDFACTORY_ASSOCIATE(ParamWires, IConfigurable);



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

public:		     // just for us lucky functions local to this file
    void set_index(int ind) { m_index = ind; }
    void set_plane(WirePlaneType_t plane) { m_plane = plane; }

};


static ParamWire* make_wire(int index, const Point& offset, const Point& pitch,
			    const Point& proto, const Ray& bounds)
{
    double number = index;
    Point pt1 = offset + number*pitch;
    Point pt2  = pt1 + proto;
    Ray wireray(pt1, pt2);

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


static void make_one_plane(WireStore& store, WirePlaneType_t plane,
			   const Ray& bounds,
			   const Ray& step)
{
    const Vector drift(-1,0,0);
    int global_count = store.size();
    Point offset = step.first;
    Vector pitch = step.second - offset;
    Vector proto = drift.cross(pitch).norm();


    std::vector<ParamWire*> all_wires;

    int pos_index = 0;
    while (true) {		// go in positive pitch direction
	ParamWire* wire = make_wire(pos_index, offset, pitch, proto, bounds);
	if (! wire) { break; }
	all_wires.push_back(wire);
	pos_index += 1;
    }

    int neg_index = -1;		// avoid doing 0 twice
    while (true) {		// go in negative pitch direction
	ParamWire* wire = make_wire(neg_index, offset, pitch, proto, bounds);
	if (! wire) { break; }
	all_wires.push_back(wire);
	neg_index -= 1;
    }

    // order by index
    std::sort(all_wires.begin(), all_wires.end(), SortByIndex());

    // load in to store and fix up index and plane
    for (int ind=0; ind<all_wires.size(); ++ind) {
	ParamWire* wire = all_wires[ind];
	wire->set_index(ind);
	wire->set_plane(plane);
	store.insert(wire);
    }
	
    //std::cerr << "Made "<<store.size()<<" wires for plane " << plane << std::endl;
    //std::cerr << "step = " << step << std::endl;
    //std::cerr << "bounds = " << bounds << std::endl;
}


Configuration ParamWires::default_configuration() const
{
    std::string json = R"(
{
"center_mm":{"x":0.0, "y":0.0, "z":0.0},
"size_mm":{"x":10.0, "y":1000.0, "z":1000.0},
"pitch_mm":{"u":3.0, "v":3.0, "w":3.0},
"angle_deg":{"u":60, "v":120, "w":0.0},
"offset_mm":{"u":0.0, "v":0.0, "w":0.0},
"plane_mm":{"u":0.0, "v":1.0, "w":-1.0}
}
)";
    return configuration_loads(json, "json");
}

void ParamWires::configure(const Configuration& cfg)
{
    // The local origin about which all else is measured.
    double cx = cfg.get<double>("center_mm.x")*units::mm;
    double cy = cfg.get<double>("center_mm.y")*units::mm;
    double cz = cfg.get<double>("center_mm.z")*units::mm;
    const Point center(cx,cy,cz);
    
    // The full width sizes
    double dx = cfg.get<double>("size_mm.x")*units::mm;
    double dy = cfg.get<double>("size_mm.y")*units::mm;
    double dz = cfg.get<double>("size_mm.z")*units::mm;
    const Point deltabb(dx,dy,dz);
    const Point bbmax = center + 0.5*deltabb;
    const Point bbmin = center - 0.5*deltabb;

    // The angles of the wires w.r.t. the positive Y axis
    double angU = cfg.get<double>("angle_deg.u")*units::degree;
    double angV = cfg.get<double>("angle_deg.v")*units::degree;
    double angW = cfg.get<double>("angle_deg.w")*units::degree;

    // The pitch magnitudes.
    double pitU = cfg.get<double>("pitch_mm.u")*units::mm;
    double pitV = cfg.get<double>("pitch_mm.v")*units::mm;
    double pitW = cfg.get<double>("pitch_mm.w")*units::mm;

    // Pitch vectors
    const Vector pU(0, pitU*std::cos(+angU), pitU*std::sin(+angU));
    const Vector pV(0, pitV*std::cos(+angV), pitV*std::sin(+angV));
    const Vector pW(0, pitW*std::cos(+angW), pitW*std::sin(+angW));

    // The offset in the pitch direction from the center to a wire
    double offU = cfg.get<double>("offset_mm.u")*units::mm;
    double offV = cfg.get<double>("offset_mm.v")*units::mm;
    double offW = cfg.get<double>("offset_mm.w")*units::mm;

    Point oU = center + pU.norm() * offU;
    Point oV = center + pV.norm() * offV;
    Point oW = center + pW.norm() * offW;

    // Force X location of plane along the X axis.
    oU.x(cfg.get<double>("plane_mm.u")*units::mm);
    oV.x(cfg.get<double>("plane_mm.v")*units::mm);
    oW.x(cfg.get<double>("plane_mm.w")*units::mm);


    const Ray bounds(bbmin, bbmax);
    const Ray U(oU, oU+pU), V(oV, oV+pV), W(oW, oW+pW);
    this->generate(bounds, U, V, W);
}

void ParamWires::generate(const Ray& bounds, 
			  const Ray& U, const Ray& V, const Ray& W)
{
    this->clear();

    // save for posterity
    m_bounds = bounds;
    m_pitchU = U;
    m_pitchV = V;
    m_pitchW = W;

    make_one_plane(m_wire_store, kUwire, bounds, U);
    make_one_plane(m_wire_store, kVwire, bounds, V);
    make_one_plane(m_wire_store, kWwire, bounds, W);
}

void ParamWires::generate(float dx, float dy, float dz,
			  float pitch, float angle)
{
    // The local origin about which all else is measured.
    const Point center;
    const Point deltabb(dx,dy,dz);
    const Point bbmax = center + 0.5*deltabb;
    const Point bbmin = center - 0.5*deltabb;

    double angU = angle;
    double angV = M_PI-angle;
    double angW = 0.0;

    // Pitch vectors
    const Vector pU(0, pitch*std::cos(+angU), pitch*std::sin(+angU));
    const Vector pV(0, pitch*std::cos(+angV), pitch*std::sin(+angV));
    const Vector pW(0, pitch*std::cos(+angW), pitch*std::sin(+angW));

    const Point oU(+0.25*dx, 0.0, 0.0);
    const Point oV( 0.0    , 0.0, 0.0);
    const Point oW(-0.25*dx, 0.0, 0.0);

    const Ray bounds(bbmin, bbmax);
    const Ray U(oU, oU+pU), V(oV, oV+pV), W(oW, oW+pW);
    this->generate(bounds, U, V, W);

}


const WireStore& ParamWires::wires() const
{
    return m_wire_store;
}


void ParamWires::clear() {
    m_wire_store.clear();
    m_bounds = m_pitchU = m_pitchV = m_pitchW = Ray();
}

ParamWires::ParamWires()
{
}

ParamWires::~ParamWires()
{
    this->clear();
}


