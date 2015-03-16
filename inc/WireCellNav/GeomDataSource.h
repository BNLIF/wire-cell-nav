#ifndef WIRECELLNAV_GEOMDATASOURCE_H
#define WIRECELLNAV_GEOMDATASOURCE_H

#include "WireCellData/Wire.h"

#include <map>

namespace WireCellNav {

    /**
       A source of wire geometry information.

     */
    class GeomDataSource {
	WireCellData::WireCollection wires;
	mutable std::map<int,int> ident2index, channel2index;
	mutable std::map<std::pair<int,int>, int> pi2index;

	// Maybe fill the cache
	bool fill_cache() const;

    public:
	GeomDataSource();
	virtual ~GeomDataSource();

	/// Add a wire object, return its ident.
	int add_wire(const WireCellData::Wire& wire);

	/// Get full collection of wires.
	const WireCellData::WireCollection get_wires() const;

	/// Look up a wire by it's identifier
	const WireCellData::Wire& by_ident(int ident) const;

	/// Look up a wire by it's electronics channel number
	const WireCellData::Wire& by_channel(int channel) const;

	/// Look up a wire by its plane number and index
	const WireCellData::Wire& by_planeindex(int plane, int index) const;

    };

}

#endif
