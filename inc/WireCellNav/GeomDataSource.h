#ifndef WIRECELLNAV_GEOMDATASOURCE_H
#define WIRECELLNAV_GEOMDATASOURCE_H

#include "WireCellData/GeomWire.h"
#include "WireCellData/Vector.h"

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
	double pitch(WirePlaneType_t plane) const;

	/// Return a Vector which points along the pitch and has unit length.
	WireCell::Vector pitch_unit_vector(WirePlaneType_t plane) const;

	/// Return the wire angle of given plane w.r.t. the Y axis (in System of Units)
	double angle(WirePlaneType_t plane) const;

	/// Return size extent in all Cartesian directions (x=0, y=1 and
	/// z=2) of all wire endpoints.  Values are length in the
	/// System of Units.  One can limit the extent to a particular
	/// wire plane
	std::vector<double> extent(WirePlaneType_t plane = kUnknownWirePlaneType) const;

	/// Return the geometric center of all wire planes.
	Vector center() const;

	/// Return min/max in a particular direction (x=0, y=1 and
	/// z=2) of all wire endpoints.  Values are length in the
	/// System of Units.  One can limit the extent to a particular
	/// wire plane
	std::pair<double, double> minmax(int axis, WirePlaneType_t plane = kUnknownWirePlaneType) const;

	/// Return true if point is contained in the extent.
	bool contained(const Vector& point) const;
	bool contained_yz(const Vector& point) const;

	/// Given a point to calculate its u-v-w position.
	double wire_dist(const Vector& point, WirePlaneType_t plane = kUnknownWirePlaneType) const;
	
	double wire_dist(const GeomWire& wire) const;
	
	/// Given two wires, calculate their crossing point projected
	/// to the y-z plane.  Only y and z values of result are modified.
	/// Return true if point is in the wire plane.
	bool crossing_point(const GeomWire& wire1, const GeomWire& wire2, 
			    Vector& result) const;

	/// Given two lines defined as distances measured
	/// perpendicular to the given wire plane type, calculate
	/// their crossing point projected to the y-z plane.  Only y
	/// and z values of result are modified.  Return true if point
	/// is in the wire plane.
	bool crossing_point(double dis1, double dis2, 
			    WirePlaneType_t plane1, WirePlaneType_t plane2, 
			    Vector& result) const;

	/// Return the two wires that bound a given point in the wire
	/// plane.  If point is out not bounded on the lower distance
	/// side the first wire pointer will be NULL, etc if the point
	/// is not bounded in the higher distance side.  This
	/// implementation assumes wires are of uniform pitch and
	/// angle.
	GeomWirePair bounds(const Vector& point, 
			    WirePlaneType_t plane = kUnknownWirePlaneType) const;

	/// Return closest wire in the plane to the given point along
	/// the direction perpendicular to the wires of the given
	/// plane.  This implementation assumes wires are of uniform
	/// pitch and angle.
	const GeomWire* closest(const Vector& point, 
				WirePlaneType_t plane = kUnknownWirePlaneType) const;
	
	/// Xin, document me.
	void avoid_gap(Vector& point) const;
	
    private:
	GeomWireSet wires;
	// reverse lookup cache of wire ID to wire object
	mutable std::map<int, const GeomWire*> ident2wire;
	// reverse lookup cache of <plane,index> to wire object
	mutable std::vector<const GeomWire*> pi2wire[3];
	// reverse lookup cache of all wire segments associated with one electronics channel
	mutable std::map<int, GeomWireSelection> channel2wire;

	// one for each x,y,z
	mutable std::map<WirePlaneType_t, std::pair<double,double> > mm_cache[3];

	mutable double angle_cache[3];

	// Maybe fill the cache
	bool fill_cache() const;
	bool fill_mm_cache() const;
	bool fill_angle_cache() const;
	

    };

}

#endif
