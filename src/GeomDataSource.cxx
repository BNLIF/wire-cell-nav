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
    
    if (angle>3.1415926/2.) angle -= 3.1415926;
    
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


float GeomDataSource::wire_dist(const Point& point, WirePlaneType_t plane) const{
  float theta = angle(plane);
  float dis = std::cos(theta/units::radian) *point.z  - std::sin(theta/units::radian) * point.y;

  return dis;
}

float GeomDataSource::wire_dist(const GeomWire& wire) const{
  float theta = angle(wire.plane());
  
  float dis = std::cos(theta/units::radian) *wire.point1().z  - std::sin(theta/units::radian) * wire.point1().y;
  dis += std::cos(theta/units::radian) * wire.point2().z  - std::sin(theta/units::radian) * wire.point2().y;
  dis = dis/2.;
  
  return dis;
}

Point GeomDataSource::crossing_point(const GeomWire& wire1, const GeomWire& wire2){
  
  float dis1 = wire_dist(wire1);
  float dis2 = wire_dist(wire2);
  
  Point p = crossing_point(dis1,dis2,wire1.plane(),wire2.plane());
  
  if (p.y >= wire1.point1().y && p.y <= wire1.point1().y  || p.y >= wire1.point2().y && p.y <= wire1.point1().y){
  }else{
    p.y=-1;
  }
  if (p.y >= wire2.point1().y && p.y <= wire2.point1().y  || p.y >= wire2.point2().y && p.y <= wire2.point1().y){
  }else{
    p.y = -1;
  }

  if (p.z >= wire1.point1().z && p.z <= wire1.point1().z  || p.z >= wire1.point2().z && p.z <= wire1.point1().z){
  }else{
    p.z=-1;
  }
  if (p.z >= wire2.point1().z && p.z <= wire2.point1().z  || p.z >= wire2.point2().z && p.z <= wire2.point1().z){
  }else{
    p.z = -1;
  }
  


  return p;
}


Point GeomDataSource::crossing_point(float dis1, float dis2, WirePlaneType_t plane1, WirePlaneType_t plane2){
  float theta1 = angle(plane1);
  float theta2 = angle(plane2);

  
  float a1 = std::cos(theta1/units::radian);
  float b1 = -std::sin(theta1/units::radian);
  
  float a2 = std::cos(theta2/units::radian);
  float b2 = -std::sin(theta2/units::radian);

  //equation array is
  // dis1 = z * a1 + y * b1;
  // dis2 = z * a2 + y * b2;
  float x = -1;
  float y,z;
  if (b1*a2-b2*a1!=0){
    y = (dis1 * a2 - dis2 *a1)/(b1*a2-b2*a1);
  }else{
    y = -1;
  }

 


  if (a1*b2 - a2 * b1!=0){
    z = (dis1 * b2 - dis2 * b1)/(a1*b2 - a2 * b1);
  }else{
    z= -1;
  }



  Point p(x,y,z);

  return p;
}




GeomWirePair GeomDataSource::bounds(const Point& point, WirePlaneType_t plane) const
{
  float theta = angle(plane);
  float dis = wire_dist(point,plane);
  
  //find out the ident number of the first wire
  const GeomWire *first_wire = by_planeindex(plane,0);
  float dis0 = wire_dist( *first_wire );
  
  int num = (dis - dis0)/pitch(plane);
  
  const GeomWire *central_wire = by_planeindex(plane,num);
  float central_dis = wire_dist(*central_wire);

  GeomWirePair p1;
  if (dis > central_dis){
    p1.first = by_planeindex(plane,num);
    p1.second = by_planeindex(plane,num+1);
  }else if (dis < central_dis){
    p1.first = by_planeindex(plane,num-1);
    p1.second = by_planeindex(plane,num);
  }else{
    if (num==0){
      p1.first = by_planeindex(plane,num);
      p1.second = by_planeindex(plane,num+1);
    }else{
      p1.first = by_planeindex(plane,num-1);
      p1.second = by_planeindex(plane,num);
    }
  }

  return p1;
  //  return GeomWirePair();
}

const GeomWire* GeomDataSource::closest(const Point& point, WirePlaneType_t plane) const
{
  // fixme: write me!
  float dis = wire_dist(point,plane);
  GeomWirePair p1 = bounds(point,plane);
  float dis1 = wire_dist(*p1.first);
  float dis2 = wire_dist(*p1.second);
  
  if (fabs(dis1-dis)<fabs(dis2-dis)){
    return p1.first;
  }else{
    return p1.second;
  }

}

