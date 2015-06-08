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

    for (int iplane=0; iplane<3; ++iplane) {
	pi2wire[iplane].clear();
    }

    int ind = 0;
    GeomWireSet::const_iterator wit, done = wires.end();
    for (wit=wires.begin(); wit != done; ++wit) {
	const GeomWire& wire = *wit;
	ident2wire[wire.ident()] = &wire;
	channel2wire[wire.channel()].push_back(&wire);
	WirePlaneIndex wpi = wire.plane_index();
	pi2wire[wpi.first].resize(wpi.second+1,0);
	pi2wire[wpi.first][wpi.second] = &wire;
    }


    // angle
    for (int iplane=0; iplane<3; ++iplane) {
	std::vector<const GeomWire*>& wip = pi2wire[iplane];
	if (! wip.size()) {
	    angle_cache[iplane] = -999;
	    continue;
	}
	const GeomWire& w = *wip[0];
	double dz = w.point2().z - w.point1().z;
	double dy = w.point2().y - w.point1().y;
	double angle = std::atan2(dz, dy);
	if (angle>3.1415926/2.) angle -= 3.1415926;
	angle_cache[iplane] = angle*units::radian;
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
	if (plane == kUnknownWirePlaneType || wire.plane() == plane) {
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
    //fill_cache();
    //return pi2wire[planeindex.first].at(planeindex.second);
    return by_planeindex(planeindex.first, planeindex.second);
    
}
const GeomWire* GeomDataSource::by_planeindex(WirePlaneType_t plane, int index) const
{
    if (0 > plane || plane >= 3) {
        return 0;
    }
    if (index < 0) {
	return 0;
    }
    fill_cache();
    std::vector<const GeomWire*>& wires = pi2wire[plane];
    if (index >= wires.size()) {
	return 0;
    }
    return wires[index];
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
    if (0 > plane || plane >= 3) {
        return -999;
    }
    fill_cache();
    return angle_cache[plane];
}


static std::pair<float,float> vmm(const std::vector<float>& v)
{
    auto res = std::minmax_element(v.begin(),v.end());
    return std::pair<float,float>( *res.first, *res.second);
}

bool GeomDataSource::fill_mm_cache() const 
{
    // std::cerr << "fill_mm_cache "
    //           << mm_cache[0].size() << ", "
    //           << mm_cache[1].size() << ", "
    //           << mm_cache[2].size()
    //           << std::endl;

    if (mm_cache[0].size()) {
	return false;
    }

    std::vector<float> totx, toty, totz;

    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t tplane = (WirePlaneType_t)iplane;
        std::vector<float> x, y, z;
	GeomWireSelection ws = this->wires_in_plane(tplane);
	size_t nwires = ws.size();
        for (size_t wind=0; wind<nwires; ++wind) {
	    const GeomWire& w = *ws[wind];

            const Point& one = w.point1();
            x.push_back(one.x);             totx.push_back(one.x); 
            y.push_back(one.y);             toty.push_back(one.y); 
            z.push_back(one.z);             totz.push_back(one.z); 
            const Point& two = w.point2();
            x.push_back(two.x);             totx.push_back(two.x); 
            y.push_back(two.y);             toty.push_back(two.y); 
            z.push_back(two.z);             totz.push_back(two.z); 
        }
	mm_cache[0][tplane] = vmm(x);
	mm_cache[1][tplane] = vmm(y);
	mm_cache[2][tplane] = vmm(z);
    }
    mm_cache[0][kUnknownWirePlaneType] = vmm(totx);
    mm_cache[1][kUnknownWirePlaneType] = vmm(toty);
    mm_cache[2][kUnknownWirePlaneType] = vmm(totz);
    
    // for (int iplane=kUnknownWirePlaneType; iplane<=kYwire; ++iplane) {
    //     WireCell::WirePlaneType_t eplane = (WireCell::WirePlaneType_t)iplane;
    //     std::cerr << "CACHE HIT: extent for plane "<< iplane << ": "
    //               << " x:" << mm_cache[0][eplane].first <<" "<<mm_cache[0][eplane].first
    //               << " y:" << mm_cache[1][eplane].first <<" "<<mm_cache[1][eplane].first
    //               << " z:" << mm_cache[2][eplane].first <<" "<<mm_cache[2][eplane].first
    //               << std::endl;
    // }
    return true;
}

Point GeomDataSource::center() const
{
    Point p;
    std::pair<float,float> mm;
    mm = minmax(0);
    p.x = 0.5*(mm.first+mm.second);
    mm = minmax(1);
    p.y = 0.5*(mm.first+mm.second);
    mm = minmax(2);
    p.z = 0.5*(mm.first+mm.second);

    return p;
}

std::vector<float> GeomDataSource::extent(WireCell::WirePlaneType_t plane) const
{
    if (plane < kUnknownWirePlaneType || plane > kYwire) {
        return std::vector<float>();
    }

    fill_mm_cache();

    std::vector<float> ret;
    for (int ind=0; ind<3; ++ind) {
	std::pair<float,float> mm = mm_cache[ind][plane];
	ret.push_back(mm.second-mm.first);
    }

    return ret;
}

std::pair<float, float> GeomDataSource::minmax(int axis, WireCell::WirePlaneType_t plane) const
{
    if (plane < kUnknownWirePlaneType || plane > kYwire) {
        return std::pair<float,float>(-999,-999);
    }

    fill_mm_cache();

    return mm_cache[axis][plane];
}

/// Return true if point is contained in the extent.
bool GeomDataSource::contained(const Point& point) const
{
    std::pair<float, float> mm;

    mm = minmax(0);
    if (point.x < mm.first || mm.second <= point.x) { 
    	return false;
    }

    mm = minmax(1);
    if (point.y < mm.first || mm.second <= point.y) { 
	return false;
    }

    mm = minmax(2);
    if (point.z < mm.first || mm.second <= point.z) { 
	return false;
    }

    return true;
}

/// Return true if point is contained in the extent.
bool GeomDataSource::contained_yz(const Point& point) const
{
    std::pair<float, float> mm;

   

    mm = minmax(1);
    if (point.y < mm.first || mm.second <= point.y) { 
	return false;
    }

    mm = minmax(2);
    if (point.z < mm.first || mm.second <= point.z) { 
	return false;
    }

    return true;
}


float GeomDataSource::wire_dist(const Point& point, WirePlaneType_t plane) const
{
    float theta = angle(plane);
    float dis = std::cos(theta/units::radian) *point.z  - std::sin(theta/units::radian) * point.y;
    
    return dis;
}

float GeomDataSource::wire_dist(const GeomWire& wire) const
{
  
    Point p = float(0.5)*(wire.point1()+wire.point2());
    return wire_dist(p, wire.plane());

    // float theta = angle(wire.plane());
    // float dis = std::cos(theta/units::radian) *wire.point1().z  - std::sin(theta/units::radian) * wire.point1().y;
    // dis += std::cos(theta/units::radian) * wire.point2().z  - std::sin(theta/units::radian) * wire.point2().y;
    // dis = dis/2.;
  
    // return dis;
}

bool GeomDataSource::crossing_point(const GeomWire& wire1, const GeomWire& wire2, 
				    Point& result) const
{
    float dis1 = wire_dist(wire1);
    float dis2 = wire_dist(wire2);
  
    bool okay = crossing_point(dis1,dis2,wire1.plane(),wire2.plane(), result);
    if (!okay) {
	return false;
    }

    // check if crossing point is bounded by Y and Z wire extent
    
    std::pair<float, float> mm;

    mm = minmax(1);		// y
    if (result.y < mm.first || mm.second <= result.y) { 
	return false;
    }

    mm = minmax(2);		// z
    if (result.z < mm.first || mm.second <= result.z) { 
	return false;
    }

    return true;
}


bool GeomDataSource::crossing_point(float dis1, float dis2,
				    WirePlaneType_t plane1, WirePlaneType_t plane2, 
				    Point& result) const
{
    float theta1 = angle(plane1);
    float theta2 = angle(plane2);
  
    float a1 = std::cos(theta1/units::radian);
    float b1 = -std::sin(theta1/units::radian);
  
    float a2 = std::cos(theta2/units::radian);
    float b2 = -std::sin(theta2/units::radian);

    //equation array is
    // dis1 = z * a1 + y * b1;
    // dis2 = z * a2 + y * b2;

    bool okay = true;

    if (b1*a2-b2*a1 == 0) {
	okay = false;
    }
    else {
	result.y = (dis1 * a2 - dis2 *a1)/(b1*a2-b2*a1);
    }

    if (a1*b2 - a2 * b1 == 0) {
	okay = false;
    }
    else{
	result.z = (dis1 * b2 - dis2 * b1)/(a1*b2 - a2 * b1);
    }
    return okay;
}




GeomWirePair GeomDataSource::bounds(const Point& point, WirePlaneType_t plane) const
{
  float theta = angle(plane);
  float dis = wire_dist(point,plane);
  
  //find out the ident number of the first wire
  const GeomWire *first_wire = by_planeindex(plane,0);
  float dis0 = wire_dist( *first_wire );
  
  int num = (dis - dis0)/pitch(plane); // first guess
  
  const GeomWire *central_wire = by_planeindex(plane,num);
  float central_dis = wire_dist(*central_wire);

  

  GeomWirePair p1;
  if (dis > central_dis){
    int num1 = (dis -central_dis)/pitch(plane);
    p1.first = by_planeindex(plane,num+num1);
    p1.second = by_planeindex(plane,num+num1+1);
    
    // if (!by_planeindex(plane,num+num1+1)){
    //   p1.second = by_planeindex(plane,num+num1+1);
    // }else{
    //   p1.second = by_planeindex(plane,num+num1);
    // }
  }else if (dis < central_dis){
    int num1 = (central_dis-dis)/pitch(plane);

    //    std::cout << num << " " << dis << " " << central_dis << " " << num1 << std::endl;
    if (num-num1-1>0){
      p1.first = by_planeindex(plane,num-num1-1);
    }else{
      p1.first = by_planeindex(plane,num-num1);
    }

    // if (!by_planeindex(plane,num-num1-1)){
    //   p1.first = by_planeindex(plane,num-num1-1);
    // }else{
    //   p1.first = by_planeindex(plane,num-num1);
    // }

    p1.second = by_planeindex(plane,num-num1);
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


void GeomDataSource::avoid_gap(Point& p) const{
    
  int flag = 0;
  float pitch1;
  // do{
  //   flag = 0;
    for (int iplane=0; iplane < 3; ++iplane) {
      WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane); // annoying
      GeomWirePair p1 = bounds(p, plane);
      float dis = wire_dist(p,plane);
      float dis1 = wire_dist(*p1.first);
      float dis2 = wire_dist(*p1.second);
      pitch1 = pitch(plane);
      std::cout << flag << " " << dis << " " << dis1 << " " << dis2 << " " << fabs(dis1+dis2-dis-dis) << " " << pitch1/20. << " " << p1.first->channel() << " " << p1.second->channel() << std::endl;
      if (fabs(dis1-dis-dis+dis2)/2.<pitch1/10.){
	flag = 1; 
      }
    }
    // if (flag==1){
    //   p.z = p.z - pitch1/10.;
    //   if (!contained_yz(p)){
    // 	p.z = p.z + pitch1/5.;
    //   }
    // }
  // }while(flag==1);
  
}
