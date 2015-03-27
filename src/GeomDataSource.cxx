#include "WireCellNav/GeomDataSource.h"

using namespace WireCell;

GeomDataSource::GeomDataSource()
{
}

GeomDataSource::~GeomDataSource()
{
}

bool GeomDataSource::fill_cache() const
{
    size_t num = wires.size();

    if (num <= ident2wire.size()) {
	return false;
    }

    ident2wire.clear();
    channel2wire.clear();
    pi2wire.clear();

    int ind = 0;
    WireSet::const_iterator wit, done = wires.end();
    for (wit=wires.begin(); wit != done; ++wit) {
	const Wire& wire = *wit;
	ident2wire[wire.ident] = &wire;
	channel2wire[wire.channel] = &wire;
	pi2wire[wire.plane_index()] = &wire;
    }
    return true;
}


void GeomDataSource::add_wire(const Wire& wire)
{
    wires.push_back(wire);
}

const WireSet& GeomDataSource::get_wires() const
{
    return wires;
}

WireSelection GeomDataSource::wires_in_plane(WirePlaneType_t plane)
{
    WireSelection ws;
    WireSet::const_iterator wit, done = wires.end();

    for (wit=wires.begin(); wit != done; ++wit) {
	const Wire& wire = *wit;
	if (plane == kUnknown or wire.plane == plane) {
	    ws.push_back(&wire);
	}
    }

    return ws;
}


const Wire* GeomDataSource::by_ident(int ident) const
{
    fill_cache();
    return ident2wire[ident];
}


const Wire* GeomDataSource::by_channel(int channel) const
{
    fill_cache();
    return channel2wire[channel];
}

const Wire* GeomDataSource::by_planeindex(const WirePlaneIndex planeindex) const
{
    fill_cache();
    return pi2wire[planeindex];
}
const Wire* GeomDataSource::by_planeindex(WirePlaneType_t plane, int index) const
{
    return by_planeindex(WirePlaneIndex(plane,index));
}

