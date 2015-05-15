#ifndef WIRECELLNAV_GEOMDATASOURCE_H
#define WIRECELLNAV_GEOMDATASOURCE_H

#include "WireCellData/GeomWire.h"

#include <map>

namespace WireCell {

    /**
       A source of wire geometry information.

     */
    class GeomDataSource {
    public:
	GeomDataSource();
	virtual ~GeomDataSource();

	/// Add a wire object, return its ident.
	void add_wire(const GeomWire& wire);

	/// Get full collection of wires.
	const GeomWireSet& get_wires() const;

	/// Return a selection of wires in the given plane/direction
	/// or all of them if no direction is specified.
	GeomWireSelection wires_in_plane(WirePlaneType_t plane = kUnknownWirePlaneType) const;

	/// Look up a wire by it's identifier
	const GeomWire* by_ident(int ident) const;

	/// Look up a wire by it's electronics channel number.
	const GeomWire* by_channel_segment(int channel, int segment) const;
	const GeomWireSelection& by_channel(int channel) const;

	/// Look up a wire by its plane number and index
	const GeomWire* by_planeindex(WirePlaneType_t plane, int index) const;
	const GeomWire* by_planeindex(const WirePlaneIndex planeindex) const;

	/// Return wire pitch (perpendicular distance between wires, in System of Units) 
	float pitch(WirePlaneType_t plane) const;

	/// Return the wire angle of given plane w.r.t. the Y axis (in System of Units)
	float angle(WirePlaneType_t plane) const;

	/// Return size extent in all Cartesian directions (x=0, y=1 and
	/// z=2) of all wire endpoints.  Values are length in the
	/// System of Units.  One can limit the extent to a particular
	/// wire plane
	std::vector<float> extent(WirePlaneType_t plane = kUnknownWirePlaneType) const;

	/// Return the geometric center of all wire planes.
	Point center() const;

	/// Return min/max in a particular direction (x=0, y=1 and
	/// z=2) of all wire endpoints.  Values are length in the
	/// System of Units.  One can limit the extent to a particular
	/// wire plane
	std::pair<float, float> minmax(int axis, WirePlaneType_t plane = kUnknownWirePlaneType) const;

	/// Return true if point is contained in the extent.
	bool contained(const Point& point) const;
	bool contained_yz(const Point& point) const;

	/// Given a point to calculate its u-v-w position.
	float wire_dist(const Point& point, WirePlaneType_t plane = kUnknownWirePlaneType) const;
	
	float wire_dist(const GeomWire& wire) const;
	
	/// Given two wires, calculate their crossing point projected
	/// to the y-z plane.  Only y and z values of result are modified.
	/// Return true if point is in the wire plane.
	bool crossing_point(const GeomWire& wire1, const GeomWire& wire2, 
			    Point& result) const;

	/// Given two lines defined as distances measured
	/// perpendicular to the given wire plane type, calculate
	/// their crossing point projected to the y-z plane.  Only y
	/// and z values of result are modified.  Return true if point
	/// is in the wire plane.
	bool crossing_point(float dis1, float dis2, 
			    WirePlaneType_t plane1, WirePlaneType_t plane2, 
			    Point& result) const;

	/// Return the two wires that bound a given point in the wire plane.
	GeomWirePair bounds(const Point& point, 
			    WirePlaneType_t plane = kUnknownWirePlaneType) const;

	/// Return closest wire in the plane to the given point
	const GeomWire* closest(const Point& point, 
				WirePlaneType_t plane = kUnknownWirePlaneType) const;
	
	
	
    private:
	GeomWireSet wires;
	// reverse lookup cache of wire ID to wire object
	mutable std::map<int, const GeomWire*> ident2wire;
	// reverse lookup cache of <plane,index> to wire object
	mutable std::vector<const GeomWire*> pi2wire[3];
	// reverse lookup cache of all wire segments associated with one electronics channel
	mutable std::map<int, GeomWireSelection> channel2wire;

	// one for each x,y,z
	mutable std::map<WirePlaneType_t, std::pair<float,float> > mm_cache[3];

	mutable float angle_cache[3];

	// Maybe fill the cache
	bool fill_cache() const;
	bool fill_mm_cache() const;
	bool fill_angle_cache() const;
	

    };

}

#endif
