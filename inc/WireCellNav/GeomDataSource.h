#ifndef WIRECELLNAV_GEOMDATASOURCE_H
#define WIRECELLNAV_GEOMDATASOURCE_H

#include "WireCellData/Wire.h"

#include <map>

namespace WireCell {

    /**
       A source of wire geometry information.

     */
    class GeomDataSource {
	WireCell::WireSet wires;
	mutable std::map<int, const WireCell::Wire*> ident2wire, channel2wire;
	mutable std::map<WireCell::WirePlaneIndex, const WireCell::Wire*> pi2wire;

	// Maybe fill the cache
	bool fill_cache() const;

    public:
	GeomDataSource();
	virtual ~GeomDataSource();

	/// Add a wire object, return its ident.
	void add_wire(const WireCell::Wire& wire);

	/// Get full collection of wires.
	const WireCell::WireSet& get_wires() const;

	/// Return an selection of wires in the given plane/direction
	/// or all of them if no direction is specified.
	WireCell::WireSelection wires_in_plane(WireCell::WirePlaneType_t plane = WireCell::kUnknown) const;

	/// Look up a wire by it's identifier
	const WireCell::Wire* by_ident(int ident) const;

	/// Look up a wire by it's electronics channel number.  Fixme:
	/// this needs to either return a vector or take an index.  N
	/// wrapped wires map to 1 channel.
	const WireCell::Wire* by_channel(int channel, int segment=0) const;

	/// Look up a wire by its plane number and index
	const WireCell::Wire* by_planeindex(WireCell::WirePlaneType_t plane, int index) const;
	const WireCell::Wire* by_planeindex(const WireCell::WirePlaneIndex planeindex) const;

	/// Return wire pitch (perpendicular distance between wires, in System of Units) 
	float pitch(WireCell::WirePlaneType_t plane) const;

	/// Return the wire angle of given plane w.r.t. the Y axis (in System of Units)
	float angle(WireCell::WirePlaneType_t plane) const;

	/// Return size extent in all Cartesian directions (x=0, y=1 and
	/// z=2) of all wire endpoints.  Values are length in the
	/// System of Units.  One can limit the extent to a particular
	/// wire plane
	std::vector<float> extent(WireCell::WirePlaneType_t plane = WireCell::kUnknown) const;

	/// Return min/max in a particular direction (x=0, y=1 and
	/// z=2) of all wire endpoints.  Values are length in the
	/// System of Units.  One can limit the extent to a particular
	/// wire plane
	std::pair<float, float> minmax(int axis, WireCell::WirePlaneType_t plane = WireCell::kUnknown) const;
    };

}

#endif
