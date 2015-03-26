#ifndef WIRECELLNAV_GEOMDATASOURCE_H
#define WIRECELLNAV_GEOMDATASOURCE_H

#include "WireCellData/Wire.h"

#include <map>

namespace WireCellNav {

    /**
       A source of wire geometry information.

     */
    class GeomDataSource {
	WireCellData::WireSet wires;
	mutable std::map<int, const WireCellData::Wire*> ident2wire, channel2wire;
	mutable std::map<WireCellData::WirePlaneIndex, const WireCellData::Wire*> pi2wire;

	// Maybe fill the cache
	bool fill_cache() const;

    public:
	GeomDataSource();
	virtual ~GeomDataSource();

	/// Add a wire object, return its ident.
	void add_wire(const WireCellData::Wire& wire);

	/// Get full collection of wires.
	const WireCellData::WireSet& get_wires() const;

	/// Return an selection of wires in the given plane/direction
	/// or all of them if no direction is specified.
	WireCellData::WireSelection wires_in_plane(WireCellData::WirePlaneType_t plane = WireCellData::kUnknown);

	/// Look up a wire by it's identifier
	const WireCellData::Wire* by_ident(int ident) const;

	/// Look up a wire by it's electronics channel number
	const WireCellData::Wire* by_channel(int channel) const;

	/// Look up a wire by its plane number and index
	const WireCellData::Wire* by_planeindex(WireCellData::WirePlaneType_t plane, int index) const;
	const WireCellData::Wire* by_planeindex(const WireCellData::WirePlaneIndex planeindex) const;

    };

}

#endif
