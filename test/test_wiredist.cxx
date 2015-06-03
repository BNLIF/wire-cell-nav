#include "WireCellNav/GeomDataSource.h"
#include "WireCellData/Units.h"

#include <iostream>

using namespace WireCell;
using namespace std;
using namespace units;

int main()
{
  GeomDataSource gds;
  WireCell::WirePlaneType_t plane = static_cast<WireCell::WirePlaneType_t>(0);

  WireCell:: GeomWire wire1(10000, plane, 0, 0,
			    WireCell::Point(-6.34706e-14*cm, 117.02*cm, 0.206489*cm),
   			    WireCell::Point(-6.33987e-14*cm, 117.36*cm, 0.793511*cm));

  WireCell:: GeomWire wire2(10001, plane, 1, 0,
			    WireCell::Point(-6.34706e-14*cm, 116.675*cm, 0.206489*cm),
   			    WireCell::Point(-6.33987e-14*cm, 117.36*cm, 1.39351	*cm));

  gds.add_wire(wire1);
  gds.add_wire(wire2);
  cout << "Plane 1: " << gds.angle(plane) << " " << 3.1415926*radian << endl;
  
  cout << gds.wire_dist(wire1)/cm << endl;
  cout << gds.wire_dist(wire2)/cm << endl;
  


  plane = static_cast<WireCell::WirePlaneType_t>(1);
  WireCell:: GeomWire wire3(20000, plane, 0, 0,
			    WireCell::Point(-0.3*cm, -115.081*cm, 0.00648934*cm),
			    WireCell::Point(-0.3*cm, -115.42*cm, 0.593511*cm));
  
  WireCell:: GeomWire wire4(20001, plane, 1, 0,
			    WireCell::Point(-0.3*cm, -114.735*cm, 0.00648934*cm),
			    WireCell::Point(-0.3*cm, -115.42*cm, 1.19351*cm));

  gds.add_wire(wire3);
  gds.add_wire(wire4);
  cout << "Plane 2: " << gds.angle(plane) << " " << 3.1415926*radian << endl;
  
  cout << gds.wire_dist(wire3)/cm << endl;
  cout << gds.wire_dist(wire4)/cm << endl;

  plane = static_cast<WireCell::WirePlaneType_t>(2);
  WireCell:: GeomWire wire5(30000, plane, 0, 0,
  			    WireCell::Point(-0.6*cm, -115.53*cm, 0.3*cm),
  			    WireCell::Point(-0.6*cm, 117.47*cm, 0.3*cm));
  
  WireCell:: GeomWire wire6(30001, plane, 1, 0,
  			    WireCell::Point(-0.6*cm, -115.53*cm, 0.6*cm),
  			    WireCell::Point(-0.6*cm, 117.47*cm, 0.6*cm));

  gds.add_wire(wire5);
  gds.add_wire(wire6);
  cout << "Plane 3: " << gds.angle(plane) << " " << 3.1415926*radian << endl;
  
  cout << gds.wire_dist(wire5)/cm << endl;
  cout << gds.wire_dist(wire6)/cm << endl;

  
  plane = static_cast<WireCell::WirePlaneType_t>(0);
  WireCell::Point p1(-1,0,100);
  cout << gds.wire_dist(p1,plane)<< endl;

  Point xpt;
  bool inside = gds.crossing_point(wire1, wire3, xpt);
  cout << inside << " " << xpt << endl;
  WireCell::Point p2(-1,-114.735*cm, 0.00648934*cm);
  cout << gds.wire_dist(p2,plane)<< " " << gds.wire_dist(wire1) << endl;
  plane = static_cast<WireCell::WirePlaneType_t>(1);
  cout << gds.wire_dist(p2,plane)<< " " << gds.wire_dist(wire3) << endl;

  cout << "Test Bounds " << endl; 
  GeomWirePair pp = gds.bounds(p2,plane);
  cout << gds.wire_dist(p2,plane)  << " " << gds.wire_dist(*pp.first) << " " << gds.wire_dist(*pp.second) << endl;
  const GeomWire * wire_closest = gds.closest(p2,plane);
  cout << gds.wire_dist(*wire_closest) << endl;

  return 0;
}
