#include "WireCellNav/GeomDataSource.h"

#include <cmath>		// std::abs() - careful not to use bare abs()
#include <algorithm>

using namespace std;
using namespace WireCell;

GeomDataSource::GeomDataSource()
  : flag_nwires_cache(0)
    //, flag_pitch_cache(0)
{
}

GeomDataSource::~GeomDataSource()
{
}

// bool GeomDataSource::fill_pitch_cache() const{
//   if (flag_pitch_cache == 0){
//     flag_pitch_cache = 1;

//     fill_nwires_cache();
    
//     for (int plane = 0; plane!=3; plane ++){
//       int nwire = (nwires_cache[int(plane)]-1)/2.;
//       const GeomWire& wire0 = *this->by_planeindex(WireCell::WirePlaneType_t(plane), nwire);
//       const GeomWire& wire1 = *this->by_planeindex(WireCell::WirePlaneType_t(plane), nwire+1);
      
//       double d = (wire0.point2().z - wire0.point1().z);
//       if (d == 0) {		// y wires
// 	pitch_cache[plane] = std::abs(wire0.point1().z - wire1.point1().z);
//       }
      
//       double n = (wire0.point2().y - wire0.point1().y);
//       double m = n/d;
//       double b0 = (wire0.point1().y - m * wire0.point1().z);
//       double b1 = (wire1.point1().y - m * wire1.point1().z);
    
//       pitch_cache[plane] = std::abs(b0-b1) / sqrt(m*m + 1);
//     }
//   }
//   return true;
// }

bool GeomDataSource::fill_nwires_cache() const{
  // number of wires
  if (flag_nwires_cache == 0 ){
    flag_nwires_cache = 1;
    nwires_cache[0] = wires_in_plane(WirePlaneType_t(0)).size();
    nwires_cache[1] = wires_in_plane(WirePlaneType_t(1)).size();
    nwires_cache[2] = wires_in_plane(WirePlaneType_t(2)).size();
    return true;
  }
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
	if (wpi.second+1 > pi2wire[wpi.first].size()) {
	    pi2wire[wpi.first].resize(wpi.second+1,0);
	}
	pi2wire[wpi.first][wpi.second] = &wire;
    }

    fill_nwires_cache();

    // angle
    for (int iplane=0; iplane<3; ++iplane) {
	std::vector<const GeomWire*>& wip = pi2wire[iplane];
	if (! wip.size()) {
	    angle_cache[iplane] = -999;
	    continue;
	}
	int nwire = (nwires_cache[int(iplane)]-1)/4.+1;
	const GeomWire& w = *wip.at(nwire); // the first wire might be biased .. use the 100th wire for now
	double dz = w.point2().z - w.point1().z;
	double dy = w.point2().y - w.point1().y;
	double angle = std::atan2(dz, dy);
	if (w.face()) angle = angle*-1.;
	if (angle>TMath::Pi()/2.) angle -= TMath::Pi();
	if (angle<-TMath::Pi()/2.) angle += TMath::Pi();
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

const GeomWireSelection& GeomDataSource::wires_in_plane(WirePlaneType_t plane) const
{
    if (mm_gwsel.find(plane) == mm_gwsel.end()) {
	GeomWireSelection ws;
	mm_gwsel[plane] = ws;
    }
    GeomWireSelection& ws = mm_gwsel[plane];
    if (ws.size()) {
	return ws;
    }

    GeomWireSet::const_iterator wit, done = wires.end();

    for (wit=wires.begin(); wit != done; ++wit) {
	const GeomWire& wire = *wit;
	if (plane == kUnknownWirePlaneType || wire.plane() == plane) {
	    ws.push_back(&wire);
	}
    }

    //cerr << "Generated " << ws.size() << " wires for plane " << plane << endl;

    return ws;
}

const GeomWireSelection& GeomDataSource::wires_in_plane(int face, WirePlaneType_t plane) const
{
  WirePlaneType_t tmp_plane = WirePlaneType_t(plane*2+face);
  if (mm_gwsel.find(tmp_plane) == mm_gwsel.end()) {
    GeomWireSelection ws;
    mm_gwsel[tmp_plane] = ws;
  }
  GeomWireSelection& ws = mm_gwsel[tmp_plane];
  //ws.clear();
  if (ws.size()) {
    return ws;
  }

    GeomWireSet::const_iterator wit, done = wires.end();

    for (wit=wires.begin(); wit != done; ++wit) {
	const GeomWire& wire = *wit;
	if (wire.plane() == plane && wire.face() == face) {
	    ws.push_back(&wire);
	}
    }

    //cerr << "Generated " << ws.size() << " wires for plane " << plane << endl;

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
    //return wires[index];
    return wires.at(index);
}

double GeomDataSource::pitch(int face, WireCell::WirePlaneType_t plane, int flag) const
{
  const GeomWireSelection& wip = wires_in_plane(face, plane);
  int nwires = wip.size();
  int num_wire = (nwires-1)/2.;

  const GeomWire& wire0 = *wip.at(num_wire);
  const GeomWire& wire1 = *wip.at(num_wire+1);

  if (flag == 0 ){
    return std::abs(wire_dist(wire0)-wire_dist(wire1));
  }else{
    return wire_dist(wire1)-wire_dist(wire0);
  }

}

double GeomDataSource::pitch(WireCell::WirePlaneType_t plane, int flag) const
{
  // fill_pitch_cache();
  // return pitch_cache[int(plane)];

  fill_nwires_cache();
  //int num_wire = (nwires_cache[int(plane)]-1)/2;
  int num_wire = (nwires_cache[int(plane)]-1)/4;
  
  const GeomWire& wire0 = *this->by_planeindex(plane, num_wire);
  const GeomWire& wire1 = *this->by_planeindex(plane, num_wire+1);

  if (flag == 0 ){
    return std::abs(wire_dist(wire0)-wire_dist(wire1));
  }else{
    return wire_dist(wire1)-wire_dist(wire0);
  }

  // double d = (wire0.point2().z - wire0.point1().z);
  // if (d == 0) {		// y wires
  //   if (flag == 0 ){
  //     return std::abs(wire0.point1().z - wire1.point1().z);
  //   }else{
  //     return (wire1.point1().z - wire0.point1().z);
  //   }
  // }
  
  // double n = (wire0.point2().y - wire0.point1().y);
  // double m = n/d;
  // double b0 = (wire0.point1().y - m * wire0.point1().z);
  // double b1 = (wire1.point1().y - m * wire1.point1().z);
  
  // if (flag == 0){
  //   return std::abs(b0-b1) / sqrt(m*m + 1);
  // }else{
  //   return (b1-b0) / sqrt(m*m + 1);
  // }
}


WireCell::Vector GeomDataSource::pitch_unit_vector(WirePlaneType_t plane) const
{
  const GeomWire& wire = *this->by_planeindex(plane, 0);
  WireCell::Vector vwire(wire.point2() - wire.point1()), ecks(1,0,0);
  WireCell::Vector vpitch = ecks.cross(vwire);
  return vpitch.norm();
}


double GeomDataSource::angle(WireCell::WirePlaneType_t plane) const
{
  if (0 > plane || plane >= 3) {
    return -999;
  }
  fill_cache();

  return angle_cache[plane];  
}


static std::pair<double,double> vmm(const std::vector<double>& v)
{
  auto res = std::minmax_element(v.begin(),v.end());
  return std::pair<double,double>( *res.first, *res.second);
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
  
  std::vector<double> totx, toty, totz;
  
  for (int iplane=0; iplane<3; ++iplane) {
    WirePlaneType_t tplane = (WirePlaneType_t)iplane;
    std::vector<double> x, y, z;
    const GeomWireSelection& ws = this->wires_in_plane(tplane);
    size_t nwires = ws.size();
    for (size_t wind=0; wind<nwires; ++wind) {
      const GeomWire& w = *ws[wind];
      
      const Vector& one = w.point1();
      x.push_back(one.x);             totx.push_back(one.x); 
      y.push_back(one.y);             toty.push_back(one.y); 
      z.push_back(one.z);             totz.push_back(one.z); 
      const Vector& two = w.point2();
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
    //               << " x:" << mm_cache[0][eplane].first <<" "<<mm_cache[0][eplane].second
    //               << " y:" << mm_cache[1][eplane].first <<" "<<mm_cache[1][eplane].second
    //               << " z:" << mm_cache[2][eplane].first <<" "<<mm_cache[2][eplane].second
    // 		  << " size: " << mm_cache[0].size()
    //               << std::endl;
    // }

    return true;
}

Vector GeomDataSource::center() const
{
    Vector p;
    std::pair<double,double> mm;
    mm = minmax(0);
    p.x = 0.5*(mm.first+mm.second);
    mm = minmax(1);
    p.y = 0.5*(mm.first+mm.second);
    mm = minmax(2);
    p.z = 0.5*(mm.first+mm.second);

    return p;
}

std::vector<double> GeomDataSource::extent(WireCell::WirePlaneType_t plane) const
{
    if (plane < kUnknownWirePlaneType || plane > kYwire) {
        return std::vector<double>();
    }

    fill_mm_cache();

    std::vector<double> ret;
    for (int ind=0; ind<3; ++ind) {
	std::pair<double,double> mm = mm_cache[ind][plane];
	ret.push_back(mm.second-mm.first);
    }

    return ret;
}

std::pair<double, double> GeomDataSource::minmax(int axis, WireCell::WirePlaneType_t plane) const
{
    if (plane < kUnknownWirePlaneType || plane > kYwire) {
        return std::pair<double,double>(-999,-999);
    }

    fill_mm_cache();

    return mm_cache[axis][plane];
}

/// Return true if point is contained in the extent.
bool GeomDataSource::contained(const Vector& point) const
{
    std::pair<double, double> mm;

    mm = minmax(0);
    if (point.x < mm.first || mm.second < point.x) { 
    	return false;
    }

    mm = minmax(1);
    if (point.y < mm.first || mm.second < point.y) { 
	return false;
    }

    mm = minmax(2);
    if (point.z < mm.first || mm.second < point.z) { 
	return false;
    }

    return true;
}

/// Return true if point is contained in the extent.
bool GeomDataSource::contained_yz(const Vector& point) const
{
    std::pair<double, double> mm;

    mm = minmax(1);
    if (point.y < mm.first || mm.second < point.y) { 
	return false;
    }

    mm = minmax(2);
    if (point.z < mm.first || mm.second < point.z) { 
	return false;
    }

    return true;
}


double GeomDataSource::wire_dist(const Vector& point, WirePlaneType_t plane) const
{
    double theta = angle(plane);
    double dis = std::cos(theta/units::radian) *point.z  - std::sin(theta/units::radian) * point.y;
    
    return dis;
}

double GeomDataSource::wire_dist(const GeomWire& wire) const
{
    Vector center = 0.5*Vector(wire.point1() + wire.point2());
    //Vector p = float(0.5)*(wire.point1()+wire.point2());
    return wire_dist(center, wire.plane());

    // float theta = angle(wire.plane());
    // float dis = std::cos(theta/units::radian) *wire.point1().z  - std::sin(theta/units::radian) * wire.point1().y;
    // dis += std::cos(theta/units::radian) * wire.point2().z  - std::sin(theta/units::radian) * wire.point2().y;
    // dis = dis/2.;
  
    // return dis;
}

double GeomDataSource::wire_dist(const Vector& pt, const GeomWire* wire) const
{
  Point pt1 = wire->point1();
  Point pt2 = wire->point2();
  double num = TMath::Abs((pt2.y-pt1.y)*pt.z-(pt2.z-pt1.z)*pt.y+pt2.z*pt1.y-pt2.y*pt1.z);
  double den = TMath::Sqrt((pt2.y-pt1.y)*(pt2.y-pt1.y)+(pt2.z-pt1.z)*(pt2.z-pt1.z));
  if (num<1.0e-6) return 0.0;
  else return num/den;
}

bool GeomDataSource::crossing_point(const GeomWire& wire1, const GeomWire& wire2, 
				    Vector& result) const
{
    double dis1 = wire_dist(wire1);
    double dis2 = wire_dist(wire2);
  
    bool okay = crossing_point(dis1,dis2,wire1.plane(),wire2.plane(), result);
    if (!okay) {
	return false;
    }

    // check if crossing point is bounded by Y and Z wire extent
    
    std::pair<double, double> mm;

    mm = minmax(1);		// y
    if (result.y < mm.first || mm.second < result.y) { 
	return false;
    }

    mm = minmax(2);		// z
    if (result.z < mm.first || mm.second < result.z) { 
	return false;
    }

    return true;
}


bool GeomDataSource::crossing_point(double dis1, double dis2,
				    WirePlaneType_t plane1, WirePlaneType_t plane2, 
				    Vector& result) const
{
    double theta1 = angle(plane1);
    double theta2 = angle(plane2);
  
    double a1 = std::cos(theta1/units::radian);
    double b1 = -std::sin(theta1/units::radian);
  
    double a2 = std::cos(theta2/units::radian);
    double b2 = -std::sin(theta2/units::radian);

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


    std::pair<double, double> mm;

    mm = minmax(1);		// y
    if (result.y < mm.first || mm.second < result.y) { 
	return false;
    }

    mm = minmax(2);		// z
    if (result.z < mm.first || mm.second < result.z) { 
	return false;
    }


    return okay;
}




GeomWirePair GeomDataSource::bounds(const Vector& point, WirePlaneType_t plane) const
{
    const GeomWireSelection& wip = wires_in_plane(plane);
    int nwires = wip.size();

    double dist = wire_dist(point, plane);

    const GeomWire *wire0 = by_planeindex(plane,0);
    double dist0 = wire_dist(*wire0);

    double find = (dist-dist0)/pitch(plane,1);

    int central_wiren;
    if (find <0 ){
      central_wiren = 0;
    }else if (find >= nwires-1){
      central_wiren = nwires -1;
    }else{
      central_wiren = round(find);
    }
    const GeomWire *central_wire = by_planeindex(plane,central_wiren);
    float central_dist = wire_dist(*central_wire);
    
    find = central_wiren + (dist - central_dist)/pitch(plane,1);

    if (find < 0) {
    	return GeomWirePair(0, wip[0]);
    }
    if (find >= nwires-1) {
    	return GeomWirePair(wip[nwires-1], 0);
    }
    
    // It is possible due to non-regular pitch and/or round-off that
    // the indexed calculation becomes off-by-one.  So, do a last
    // ditch check to confirm we return what is actually the closest
    // wire.
    const GeomWire *central_wire1 = by_planeindex(plane,int(find));
    central_dist = wire_dist(*central_wire1);
    if (central_dist < dist){
      return GeomWirePair(wip[int(find)], wip[int(find+1)]);
    }else{
      if (int(find-1)<0){
	return GeomWirePair(0, wip[0]);
      }else{
	return GeomWirePair(wip[int(find-1)], wip[int(find)]);
      }
      
    }
}

const GeomWire* GeomDataSource::GeomDataSource::get_ad_wire(const GeomWire *wire0, int kk,
			    WirePlaneType_t plane,
			    int face) const{
  if (plane != (WirePlaneType_t)(0) &&
      plane != (WirePlaneType_t)(1) &&
      plane != (WirePlaneType_t)(2)) {
    plane = kUnknownWirePlaneType;
  }
  const GeomWireSelection& wip = wires_in_plane(face, plane);
  int nwires = wip.size();
  auto it = find(wip.begin(),wip.end(),wire0) + kk;
  if (it >= wip.begin() && it < wip.end())
    return *it;
}

GeomWirePair GeomDataSource::GeomDataSource::bounds(const Vector& point, WirePlaneType_t plane, int face) const
{
    if (plane != (WirePlaneType_t)(0) &&
	plane != (WirePlaneType_t)(1) &&
	plane != (WirePlaneType_t)(2)) {
        plane = kUnknownWirePlaneType;
    }
    const GeomWireSelection& wip = wires_in_plane(face, plane);
    int nwires = wip.size();
    
    double dist_p = wire_dist(point, plane);
    const GeomWire *wire0 = wip.at(0);
    double dist0 = wire_dist(*wire0);
    double find = (dist_p-dist0)/pitch(face,plane,1);

    //std::cout << dist_p << " " << dist0 << " " << find << " " << plane << " " << pitch(WirePlaneType_t(0)) << " " << pitch(WirePlaneType_t(1)) << " " << pitch(WirePlaneType_t(2)) << std::endl;
    
    int central_wiren;
    if (find <0 ){
      central_wiren = 0;
    }else if (find >= nwires-1){
      central_wiren = nwires -1;
    }else{
      central_wiren = round(find);
    }
    const GeomWire *central_wire = wip.at(central_wiren);//by_planeindex(plane,central_wiren);
    float central_dist = wire_dist(*central_wire);
    
    find = central_wiren + (dist_p - central_dist)/pitch(face,plane,1);

    if (find < 0) {
      return GeomWirePair(0, wip[0]);
    }
    if (find >= nwires-1) {
      return GeomWirePair(wip[nwires-1], 0);
    }
    
    // It is possible due to non-regular pitch and/or round-off that
    // the indexed calculation becomes off-by-one.  So, do a last
    // ditch check to confirm we return what is actually the closest
    // wire.
    const GeomWire *central_wire1 = wip.at(int(find));//by_planeindex(plane,int(find));
    central_dist = wire_dist(*central_wire1);
    if (central_dist < dist_p){
      return GeomWirePair(wip[int(find)], wip[int(find+1)]);
    }else{
      if (int(find-1)<0){
    	return GeomWirePair(0, wip[0]);
      }else{
    	return GeomWirePair(wip[int(find-1)], wip[int(find)]);
      }
    }

    // GeomWireSelection::const_iterator wit, done = wip.end();
    // double dist[]={9999.,9999.}; //dist[0]: shortest distance; dist[1]: second shortest distance
    // // GeomWirePair pr(wip.at(0),wip.at(1));
    // GeomWirePair pr;
    // for (wit = wip.begin(); wit != done; ++wit) {
    //   double tmp_dist = wire_dist(point, *wit);
    //   //  std::cout << wit - wip.begin() << " " << tmp_dist << std::endl;
    //   if (dist[1]>tmp_dist) {
    // 	if (dist[0] > tmp_dist) {
    // 	  dist[0] = tmp_dist;
    // 	  pr.first = *wit;
    // 	} else {
    // 	  dist[1] = tmp_dist;
    // 	  pr.second = *wit;
    // 	}
    //   }
    // }
    // return pr;
}


const GeomWire* GeomDataSource::closest(const Vector& point, WirePlaneType_t plane, int face) const
{
  float dis = wire_dist(point,plane);
  GeomWirePair p1;
  if (face == -999) {
      p1 = bounds(point, plane);
  } else {
      p1 = bounds(point, plane, face);
  }
  float dis1,dis2;
  if (p1.first !=0){
    dis1 = wire_dist(*p1.first);
  }else{
    dis1 = -1000;
  }
  if (p1.second!=0){
    dis2 = wire_dist(*p1.second);
  }else{
    dis2 = -1000;
  }
  
  //  std::cout << dis1 << " " << dis << " " << dis2 << " " << p1.first->ident() << " " << p1.second->ident() <<  std::endl;

  if (fabs(dis1-dis)<fabs(dis2-dis)){
    return p1.first;
  }
  return p1.second;
}

void GeomDataSource::avoid_gap(Vector& p) const{
    
  int flag = 0;
  double pitch1;
  // do{
  //   flag = 0;
    for (int iplane=0; iplane < 3; ++iplane) {
      WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane); // annoying
      GeomWirePair p1 = bounds(p, plane);
      double dis = wire_dist(p,plane);
      double dis1 = wire_dist(*p1.first);
      double dis2 = wire_dist(*p1.second);
      pitch1 = pitch(plane);
      //std::cout << flag << " " << dis << " " << dis1 << " " << dis2 << " " << fabs(dis1+dis2-dis-dis) << " " << pitch1/20. << " " << p1.first->channel() << " " << p1.second->channel() << std::endl;
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
