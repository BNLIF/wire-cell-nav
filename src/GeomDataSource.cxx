#include "WireCellNav/GeomDataSource.h"


WireCellNav::GeomDataSource::GeomDataSource()
{
}

WireCellNav::GeomDataSource::~GeomDataSource()
{
}

bool WireCellNav::GeomDataSource::fill_cache() const
{
    size_t num = wires.size();

    if (num <= ident2index.size()) {
	return false;
    }

    for (size_t ind=0; ind<num; ++ind) {
	const WireCellData::Wire& wire = wires[ind];
	ident2index[wire.ident] = ind;
	channel2index[wire.ident] = ind;
	pi2index[std::pair<int,int>(wire.plane, wire.index)] = ind;
    }
    return true;
}


int WireCellNav::GeomDataSource::add_wire(const WireCellData::Wire& wire)
{
    wires.push_back(wire);
}

const WireCellData::WireCollection WireCellNav::GeomDataSource::get_wires() const
{
    return wires;
}


const WireCellData::Wire& WireCellNav::GeomDataSource::by_ident(int ident) const
{
    fill_cache();
    int ind = ident2index[ident];
    return wires[ind];
}


const WireCellData::Wire& WireCellNav::GeomDataSource::by_channel(int channel) const
{
    fill_cache();
    int ind = channel2index[channel];
    return wires[ind];
}

const WireCellData::Wire& WireCellNav::GeomDataSource::by_planeindex(int plane, int index) const
{
    fill_cache();
    int ind = pi2index[std::pair<int,int>(plane,index)];
    return wires[ind];
}

