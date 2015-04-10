#include "WireCellNav/GeomDataSource.h"

#include <cmath>		// std::abs() - careful not to use bare abs()
#include <algorithm>

using namespace WireCell;

GeomDataSource::GeomDataSource()
{
}

GeomDataSource::~GeomDataSource()
{
}

bool GeomDataSource::fill_cache() const
{
    size_t num = wires.size();

    if (num <= ident2wire.size()) {
	return false;
    }

    ident2wire.clear();
    channel2wire.clear();
    pi2wire.clear();

    int ind = 0;
    GeomWireSet::const_iterator wit, done = wires.end();
    for (wit=wires.begin(); wit != done; ++wit) {
	const GeomWire& wire = *wit;
	ident2wire[wire.ident()] = &wire;
	channel2wire[wire.channel()].push_back(&wire);
	pi2wire[wire.plane_index()] = &wire;
    }
    return true;
}


void GeomDataSource::add_wire(const GeomWire& wire)
{
    wires.insert(wire);
}

const GeomWireSet& GeomDataSource::get_wires() const
{
    return wires;
}

GeomWireSelection GeomDataSource::wires_in_plane(WirePlaneType_t plane) const
{
    GeomWireSelection ws;
    GeomWireSet::const_iterator wit, done = wires.end();

    for (wit=wires.begin(); wit != done; ++wit) {
	const GeomWire& wire = *wit;
	if (plane == kUnknownWirePlaneType or wire.plane() == plane) {
	    ws.push_back(&wire);
	}
    }

    return ws;
}


const GeomWire* GeomDataSource::by_ident(int ident) const
{
    fill_cache();
    return ident2wire[ident];
}


const std::vector<const WireCell::GeomWire*>& GeomDataSource::by_channel(int channel) const
{
    fill_cache();
    return channel2wire[channel];
}

const GeomWire* GeomDataSource::by_channel_segment(int channel, int segment) const
{
    return by_channel(channel)[segment];
}


const GeomWire* GeomDataSource::by_planeindex(const WirePlaneIndex planeindex) const
{
    fill_cache();
    return pi2wire[planeindex];
}
const GeomWire* GeomDataSource::by_planeindex(WirePlaneType_t plane, int index) const
{
    return by_planeindex(WirePlaneIndex(plane,index));
}

float GeomDataSource::pitch(WireCell::WirePlaneType_t plane) const
{
    const GeomWire& wire0 = *this->by_planeindex(plane, 0);
    const GeomWire& wire1 = *this->by_planeindex(plane, 1);

    double d = (wire0.point2().z - wire0.point1().z);
    if (d == 0) {		// y wires
	return std::abs(wire0.point1().z - wire1.point1().z);
    }

    double n = (wire0.point2().y - wire0.point1().y);
    double m = n/d;
    double b0 = (wire0.point1().y - m * wire0.point1().z);
    double b1 = (wire1.point1().y - m * wire1.point1().z);

    return std::abs(b0-b1) / sqrt(m*m + 1);
}

float GeomDataSource::angle(WireCell::WirePlaneType_t plane) const
{
    const GeomWire& w = *this->by_planeindex(plane, 0);
    double dz = w.point2().z - w.point1().z;
    double dy = w.point2().y - w.point1().y;
    double angle = std::atan2(dz, dy);
    return angle*units::radian;
}

std::vector<float> GeomDataSource::extent(WireCell::WirePlaneType_t plane) const
{
    float xmin, ymin, zmin, xmax, ymax, zmax;

    GeomWireSelection ws = this->wires_in_plane(plane);
    size_t nwires = ws.size();
    for (size_t wind=0; wind<nwires; ++wind) {
	const GeomWire& w = *ws[wind];
	float this_xmin = std::min(w.point1().x, w.point2().x);
	float this_ymin = std::min(w.point1().y, w.point2().y);
	float this_zmin = std::min(w.point1().z, w.point2().z);

	float this_xmax = std::max(w.point1().x, w.point2().x);
	float this_ymax = std::max(w.point1().y, w.point2().y);
	float this_zmax = std::max(w.point1().z, w.point2().z);

	if (!wind) {		// first time through
	    xmin = this_xmin;
	    ymin = this_ymin;
	    zmin = this_zmin;

	    xmax = this_xmax;
	    ymax = this_ymax;
	    zmax = this_zmax;
	    continue;
	}
	xmin = std::min(xmin, this_xmin);
	ymin = std::min(ymin, this_ymin);
	zmin = std::min(zmin, this_zmin);

	xmax = std::max(xmax, this_xmax);
	ymax = std::max(ymax, this_ymax);
	zmax = std::max(zmax, this_zmax);
    }	
    float ex[] = {xmax-xmin, ymax-ymin, zmax-zmin};
    return std::vector<float>(ex, ex + sizeof(ex)/sizeof(float));
}

// fixme: it is colossally lame of me to cut-and-paste the above, refactor and use caching. 

std::pair<float, float> GeomDataSource::minmax(int axis, WireCell::WirePlaneType_t plane) const
{
    float xmin, ymin, zmin, xmax, ymax, zmax;

    GeomWireSelection ws = this->wires_in_plane(plane);
    size_t nwires = ws.size();
    for (size_t wind=0; wind<nwires; ++wind) {
	const GeomWire& w = *ws[wind];
	float this_xmin = std::min(w.point1().x, w.point2().x);
	float this_ymin = std::min(w.point1().y, w.point2().y);
	float this_zmin = std::min(w.point1().z, w.point2().z);

	float this_xmax = std::max(w.point1().x, w.point2().x);
	float this_ymax = std::max(w.point1().y, w.point2().y);
	float this_zmax = std::max(w.point1().z, w.point2().z);

	if (!wind) {		// first time through
	    xmin = this_xmin;
	    ymin = this_ymin;
	    zmin = this_zmin;

	    xmax = this_xmax;
	    ymax = this_ymax;
	    zmax = this_zmax;
	    continue;
	}
	xmin = std::min(xmin, this_xmin);
	ymin = std::min(ymin, this_ymin);
	zmin = std::min(zmin, this_zmin);

	xmax = std::max(xmax, this_xmax);
	ymax = std::max(ymax, this_ymax);
	zmax = std::max(zmax, this_zmax);
    }	
    float ex[] = {xmax-xmin, ymax-ymin, zmax-zmin};

    switch (axis) {
    case 0:
	return std::pair<float,float>(xmin, xmax);
    case 1:
	return std::pair<float,float>(ymin, ymax);
    case 2: 
	return std::pair<float,float>(zmin, zmax);
    }
    return std::pair<float,float>(0,0);
}
GeomWirePair GeomDataSource::bounds(const Point& point, WirePlaneType_t plane) const
{
    // fixme: write me!
    return GeomWirePair();
}

const GeomWire* GeomDataSource::closest(const Point& point, WirePlaneType_t plane) const
{
    // fixme: write me!
    return 0;
}



float GeomDataSource::find_uvw(const Point& point, WirePlaneType_t plane){
  //fixme: wirte me!
  return 0;
}

std::pair<float,float> GeomDataSource::find_xy(GeomWire* wire1, GeomWire* wire2){
  //fixme: write me!
  return std::pair<float,float>(0,0);
}
