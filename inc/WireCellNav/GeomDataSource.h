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
	WireCell::WireSelection wires_in_plane(WireCell::WirePlaneType_t plane = WireCell::kUnknown);

	/// Look up a wire by it's identifier
	const WireCell::Wire* by_ident(int ident) const;

	/// Look up a wire by it's electronics channel number
	const WireCell::Wire* by_channel(int channel) const;

	/// Look up a wire by its plane number and index
	const WireCell::Wire* by_planeindex(WireCell::WirePlaneType_t plane, int index) const;
	const WireCell::Wire* by_planeindex(const WireCell::WirePlaneIndex planeindex) const;

    };

}

#endif
