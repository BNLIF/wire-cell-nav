#ifndef WIRECELLNAV_WIREDATABASE
#define WIRECELLNAV_WIREDATABASE

#include "WireCellIface/IWireDatabase.h"

#include <unordered_map>

namespace WireCell {

    /** This provides a way to query a set of wires beyond the simple
     * iteration that a WireCell::WireStore provides.*/
    class WireDatabase : public IWireDatabase {

    public:
	WireDatabase();
	virtual ~WireDatabase();
	
	/// Load the wire store into this database.
	void load(const WireCell::WireStore& wires);

	/// Return a collection of wires in the given plane/direction
	/// or all of them if no direction is specified.
	const WireVector& wires_in_plane(WirePlaneType_t plane = kUnknownWirePlaneType) const; 

	/// Look up a wire by it's identifier
	const IWire* by_ident(int ident) const;

	/// Look up a wire by it's electronics channel number.
	const IWire* by_channel_segment(int channel, int segment) const;

	/// Return all wires connected to a given channel
	const WireVector& by_channel(int channel) const;

	/// Look up a wire by its plane number and index
	const IWire* by_planeindex(WirePlaneType_t plane, int index) const;
	const IWire* by_planeindex(const WirePlaneIndex& planeindex) const;

	/// Return wire characteristic/average pitch (perpendicular
	/// distance between wires) as a distance in the System of Units.
	double pitch(WirePlaneType_t plane) const;

	/// Return a characteristic/average vector which points along
	/// the pitch and has unit length.
	WireCell::Point pitch_unit_vector(WirePlaneType_t plane) const;

	/// Return a characteristic/average wire angle of given plane
	/// w.r.t. the Y axis (angle is in System of Units)
	double angle(WirePlaneType_t plane) const;

	/// Return the bounding box of the wire planes as a raw
	/// between two diagonal corners.
	WireCell::Ray bounding_box() const;

	/// Return the geometric center of all wire planes.
	WireCell::Point center() const;

	/// Return the distance from the center of the first wire to
	/// the given point projected along the direction of the
	/// plane's wire pitch.
	double wire_dist(const Point& point, WirePlaneType_t plane) const;
	
	/// Return the distance from the center of the first wire to
	/// the center of the given wire projected along the direction
	/// of the plane's wire pitch.
	double wire_dist(const IWire& wire) const;
	
	/// Given two wires, calculate their crossing point projected
	/// to the y-z plane.  Only y and z values of result are modified.
	/// Return true if point is in the extent.
	bool crossing_point(const IWire& wire1, const IWire& wire2, 
				    WireCell::Point& result) const;

	/// Given two lines defined as distances measured
	/// perpendicular to the given wire plane type, calculate
	/// their crossing point projected to the y-z plane.  Only y
	/// and z values of result are modified.  Return true if point
	/// is in the extent.
	bool crossing_point(double dis1, double dis2, 
				    WirePlaneType_t plane1, WirePlaneType_t plane2, 
				    WireCell::Point& result) const;

	/// Return the two wires that bound a given point in the wire
	/// plane.  If point is out not bounded on the lower distance
	/// side the first wire pointer will be NULL, etc if the point
	/// is not bounded in the higher distance side.  This
	/// implementation assumes wires are of uniform pitch and
	/// angle.
	WirePair bounding_wires(const WireCell::Point& point, 
				WirePlaneType_t plane = kUnknownWirePlaneType) const;

	/// Return closest wire in the plane to the given point along
	/// the direction perpendicular to the wires of the given
	/// plane.  This implementation assumes wires are of uniform
	/// pitch and angle.
	const IWire* closest(const WireCell::Point& point, 
				     WirePlaneType_t plane = kUnknownWirePlaneType) const;
	
	
    private:


	std::unordered_map<int, const IWire*> m_ident2wire;
	std::unordered_map<int, WireVector> m_chan2wires;
	WireVector m_pi2wires[3];
	WireVector m_all_wires;	// keep this separately from pi2wires
				// so we can return a const ref.
	double m_angles[3];
	Vector m_pitches[3];
	Ray m_bbox;

    };


}

#endif
