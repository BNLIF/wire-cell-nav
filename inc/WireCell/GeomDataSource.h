#ifndef WIRECELL_GEOMDATASOURCE_H
#define WIRECELL_GEOMDATASOURCE_H

#include "WireCell/Wire.h"

#include <map>

namespace WireCell {

    /**
       A source of wire geometry information.

     */
    class GeomDataSource {
	WireCell::WireCollection wires;
	mutable std::map<int,int> ident2index, channel2index;
	mutable std::map<std::pair<int,int>, int> pi2index;

	// Maybe fill the cache
	bool fill_cache() const;

    public:
	GeomDataSource();
	virtual ~GeomDataSource();

	/// Add a wire object, return its ident.
	int add_wire(const WireCell::Wire& wire);

	/// Get full collection of wires.
	const WireCell::WireCollection get_wires() const;

	/// Look up a wire by it's identifier
	const WireCell::Wire& by_ident(int ident) const;

	/// Look up a wire by it's electronics channel number
	const WireCell::Wire& by_channel(int channel) const;

	/// Look up a wire by its plane number and index
	const WireCell::Wire& by_planeindex(int plane, int index) const;

    };

}

#endif
