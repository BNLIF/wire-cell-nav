#include "WireCellNav/GeomDataSource.h"

using namespace WireCellData;

WireCellNav::GeomDataSource::GeomDataSource()
{
}

WireCellNav::GeomDataSource::~GeomDataSource()
{
}

bool WireCellNav::GeomDataSource::fill_cache() const
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
	const WireCellData::Wire& wire = *wit;
	ident2wire[wire.ident] = &wire;
	channel2wire[wire.channel] = &wire;
	pi2wire[wire.plane_index()] = &wire;
    }
    return true;
}


void WireCellNav::GeomDataSource::add_wire(const WireCellData::Wire& wire)
{
    wires.push_back(wire);
}

const WireCellData::WireSet& WireCellNav::GeomDataSource::get_wires() const
{
    return wires;
}

WireCellData::WireSelection WireCellNav::GeomDataSource::wires_in_plane(WireCellData::WirePlaneType_t plane)
{
    WireSelection ws;
    WireSet::const_iterator wit, done = wires.end();

    for (wit=wires.begin(); wit != done; ++wit) {
	const WireCellData::Wire& wire = *wit;
	if (plane == kUnknown or wire.plane == plane) {
	    ws.push_back(&wire);
	}
    }

    return ws;
}


const WireCellData::Wire* WireCellNav::GeomDataSource::by_ident(int ident) const
{
    fill_cache();
    return ident2wire[ident];
}


const WireCellData::Wire* WireCellNav::GeomDataSource::by_channel(int channel) const
{
    fill_cache();
    return channel2wire[channel];
}

const WireCellData::Wire* WireCellNav::GeomDataSource::by_planeindex(const WireCellData::WirePlaneIndex planeindex) const
{
    fill_cache();
    return pi2wire[planeindex];
}
const WireCellData::Wire* WireCellNav::GeomDataSource::by_planeindex(WireCellData::WirePlaneType_t plane, int index) const
{
    return by_planeindex(WirePlaneIndex(plane,index));
}

