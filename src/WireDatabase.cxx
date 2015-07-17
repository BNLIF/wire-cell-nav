#include "WireCellNav/WireDatabase.h"
#include "WireCellUtil/Units.h"

#include <cmath>		// std::abs() - careful not to use bare abs()
#include <algorithm>

using namespace std;
using namespace WireCell;

WireDatabase::WireDatabase()
{
}

WireDatabase::~WireDatabase()
{
}

/// Load the wire store into this database.
void WireDatabase::load(const WireCell::WireSet& wires)
{
//    auto res = std::minmax_element(v.begin(),v.end());
//    return std::pair<double,double>( *res.first, *res.second);

    typedef vector<double> DVec;
    typedef DVec::iterator DVIt;
    DVec all_xyz[3];

    // cache all reverse lookups.
    int nwires = 0;
    for (auto wit = wires.begin(); wit != wires.end(); ++wit, ++nwires) {
	Wire wire = *wit;

	m_ident2wire[wire->ident()] = wire;
	m_chan2wires[wire->channel()].push_back(wire);

	m_pi2wires[wire->iplane()].resize(wire->index()+1,0);
	m_pi2wires[wire->iplane()][wire->index()] = wire;

	Ray seg = wire->ray();
	for (int ind=0; ind<3; ++ind) { // xyz
	    all_xyz[ind].push_back(seg.first[ind]);
	    all_xyz[ind].push_back(seg.second[ind]);
	}
    }    
    m_all_wires.reserve(nwires);
    
    // figure out boundary of wire planes
    for (int ind=0; ind<3; ++ind) { // xyz
	pair<DVIt, DVIt> mm = minmax_element(all_xyz[ind].begin(),
					     all_xyz[ind].end());
	m_bbox.first[ind] = *(mm.first);   // min
	m_bbox.second[ind] = *(mm.second); // max
    }

    // pre cache angle
    for (int iplane=0; iplane<3; ++iplane) {
	WireVector& wires = m_pi2wires[iplane];
	int nwires = wires.size();
	if (! nwires) {
	    m_angles[iplane] = -999;
	    continue;
	}

	m_all_wires.insert(m_all_wires.end(), wires.begin(), wires.end());

	// pick a wire half way in to the plane to be characteristic
	// in order to avoid round off error that occurs if one picks
	// the first wire which tends to be short..
	int half_way = nwires/2; // integer
	Wire w1 = wires[half_way];
	Ray seg1 = w1->ray();

	// calculate angle
	double dz = seg1.second.z() - seg1.first.z();
	double dy = seg1.second.y() - seg1.first.y();
	double angle = std::atan2(dz, dy);
	if (angle > M_PI_2) {	// why do we want it smaller than 180deg?
	    angle -= M_PI;
	}
	m_angles[iplane] = angle*units::radian;


	// pick next wire
	Wire w2 = wires[half_way+1];
	Ray seg2 = w2->ray();

	// calculate pitch vector
	const Vector vdir = (seg1.second - seg1.first).norm();
	const Vector diff = seg2.first - seg1.first;
	const Vector parr = vdir.dot(diff) * vdir;
	const Vector perp = diff - parr;
	m_pitches[iplane] = perp;
    }
}


const WireVector& WireDatabase::wires_in_plane(WirePlaneType_t plane) const
{
    if (plane == kUnknownWirePlaneType) {
	return m_all_wires;
    }
    int iplane = plane - kFirstPlane;
    return m_pi2wires[iplane];
}

Wire WireDatabase::by_ident(int ident) const
{
    auto got = m_ident2wire.find(ident);
    if (got == m_ident2wire.end()) {
	return 0;
    }
    return got->second;
}


Wire WireDatabase::by_channel_segment(int channel, int segment) const
{
    if (segment<0) { return 0; }
    const WireVector& segments = by_channel(channel);
    if (segment >= segments.size()) { return 0; }
    return segments[segment];
}

const WireVector& WireDatabase::by_channel(int channel) const
{
    auto got = m_chan2wires.find(channel);
    if (got == m_chan2wires.end()) {
	static WireVector bogus;
	return bogus;
    }
    return got->second;
}

Wire WireDatabase::by_planeindex(WirePlaneType_t plane, int index) const
{
    if (index<0) { return 0; }
    const WireVector& wires = wires_in_plane(plane);
    if (index >= wires.size()) { return 0; }
    return wires[index];
}
Wire WireDatabase::by_planeindex(const WirePlaneIndex& planeindex) const
{
    return by_planeindex(planeindex.first, planeindex.second);
}

double WireDatabase::pitch(WirePlaneType_t plane) const
{
    if (plane < kFirstPlane || plane > kLastPlane) { return 0; }
    return m_pitches[plane].magnitude();
}


WireCell::Vector WireDatabase::pitch_unit_vector(WirePlaneType_t plane) const
{
    if (plane < kFirstPlane || plane > kLastPlane) { return Vector(); }
    return m_pitches[plane].norm();
}

double WireDatabase::angle(WirePlaneType_t plane) const
{
    if (plane < kFirstPlane || plane > kLastPlane) { -999.; }
    return m_angles[plane];
}

WireCell::Ray WireDatabase::bounding_box() const
{
    return m_bbox;
}

WireCell::Point WireDatabase::center() const
{
    Point res;
    for (int ind=0; ind<3; ++ind) {
	res[ind] = 0.5 * (m_bbox.first[ind] + m_bbox.second[ind]);
    }
    return res;
}

double WireDatabase::wire_dist(const Point& point, WirePlaneType_t plane) const
{
    const WireVector& wip = wires_in_plane(plane);
    Wire wire0 = wip[0];
    Point origin = wire0->center();

    Vector vdif = point - origin;
    Vector pitch_dir = pitch_unit_vector(plane);
    return pitch_dir.dot(vdif);
}
	
double WireDatabase::wire_dist(Wire wire) const
{
    // coincidental optimization.
    if (!wire->index()) { return 0.0; }

    return wire_dist(wire->center(), wire->plane());
}
	
bool WireDatabase::crossing_point(Wire wire1, Wire wire2, 
				  WireCell::Point& result) const
{
    double dis1 = wire_dist(wire1);
    double dis2 = wire_dist(wire2);
  
    bool okay = crossing_point(dis1, dis2, wire1->plane(), wire2->plane(), result);
    if (!okay) {
	return false;
    }

    // check if crossing point is bounded by Y and Z wire extent
    if (!point_contained(result, m_bbox, 1)) { return false; }
    if (!point_contained(result, m_bbox, 2)) { return false; }
    return true;
}


bool WireDatabase::crossing_point(double dis1, double dis2, 
				  WirePlaneType_t plane1, WirePlaneType_t plane2, 
				  WireCell::Point& result) const
{
    double theta1 = angle(plane1);
    double theta2 = angle(plane2);
  
    double a1 = std::cos(theta1/units::radian);
    double b1 = -std::sin(theta1/units::radian);
  
    double a2 = std::cos(theta2/units::radian);
    double b2 = -std::sin(theta2/units::radian);

    bool okay = true;
    if (b1*a2-b2*a1 == 0) {
	okay = false;
    }
    else {
	result.y((dis1 * a2 - dis2 *a1)/(b1*a2-b2*a1));
    }

    if (a1*b2 - a2 * b1 == 0) {
	okay = false;
    }
    else {
	result.z((dis1 * b2 - dis2 * b1)/(a1*b2 - a2 * b1));
    }
    return okay;
}




WirePair WireDatabase::bounding_wires(const WireCell::Point& point,
				      WirePlaneType_t plane) const
{
    const WireVector& wip = wires_in_plane(plane);
    int nwires = wip.size();

    double dist = wire_dist(point, plane);

    Wire wire0 = by_planeindex(plane,0);
    double dist0 = wire_dist(wire0);

    double find = (dist-dist0)/pitch(plane);
    int central_wiren = 0;
    if (find < 0) {
	central_wiren = 0;
    }
    else if (find >= nwires - 1) {
	central_wiren = nwires - 1;
    }
    else{
	central_wiren = round(find);
    }
    Wire central_wire = by_planeindex(plane,central_wiren);
    double central_dist = wire_dist(central_wire);
    
    find = central_wiren + (dist - central_dist)/pitch(plane);

    if (find < 0) {
        return WirePair(0, wip[0]);
    }
    if (find >= nwires-1) {
        return WirePair(wip[nwires-1], 0);
    }
    
    // It is possible due to non-regular pitch and/or round-off that
    // the indexed calculation becomes off-by-one.  So, do a last
    // ditch check to confirm we return what is actually the closest
    // wire.
    Wire central_wire1 = by_planeindex(plane,int(find));

    central_dist = wire_dist(central_wire1);
    if (central_dist < dist){
	return WirePair(wip[int(find)], wip[int(find+1)]);
    }

    if ( int(find-1) < 0) {
        return WirePair(0, wip[0]);
    }

    return WirePair(wip[int(find-1)], wip[int(find)]);
}


Wire WireDatabase::closest(const WireCell::Point& point, 
			   WirePlaneType_t plane) const
{
    WirePair wp = bounding_wires(point,plane);

    // first take care of case where we are outside wire plane
    if (!wp.first) {
	return wp.second;
    }
    if (!wp.second) {
	return wp.first;
    }

    // If actually between two wires, see which is closer
    double dis1 = wire_dist(wp.first);
    double dis2 = wire_dist(wp.second);
    float dis = wire_dist(point, plane);

    if (fabs(dis1-dis) < fabs(dis2-dis)){
	return wp.first;
    }
    return wp.second;
}


