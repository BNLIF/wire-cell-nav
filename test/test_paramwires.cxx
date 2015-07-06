#include "WireCellNav/ParamWires.h"
#include "WireCellNav/GeomDataSource.h"
#include "WireCellData/Units.h"


#include <iostream>
#include <cmath>
#include <cassert>

using namespace WireCell;
using namespace std;

int main()
{
    float dx=10*units::mm;
    float dy=1*units::meter;
    float dz=1*units::meter;
    float pitch = 100.0*units::mm;
    float nudge = 0.5*pitch;
    float angle = 60.0*units::pi/180.0;

    Vector bmin(-0.5*dx, -0.5*dy, -0.5*dz);
    Vector bmax(+0.5*dx, +0.5*dy, +0.5*dz);
    Vector drift(1,0,0);

    Vector oU(0.2*dx, -0.5*dy + nudge, -0.5*dz + nudge);
    Vector oV(0.3*dx, -0.5*dy + nudge, +0.5*dz - nudge);
    Vector oW(0.1*dx,           nudge, -0.5*dz + nudge);

    Vector pU(0, pitch*std::cos(+angle), pitch*std::sin(+angle));
    Vector pV(0, pitch*std::cos(-angle), pitch*std::sin(-angle));
    Vector pW(0, 0, pitch);


    cerr << "bmin=" << bmin
	 << " bmax=" << bmax
	 << " drift=" << drift << endl;
    cerr << "Uoffset=" << oU << " Upitch=" << pU << endl;
    cerr << "Voffset=" << oV << " Vpitch=" << pV << endl;
    cerr << "Woffset=" << oW << " Wpitch=" << pW << endl;

    ParamWires wires(bmin,bmax,drift, oU,pU, oV,pV, oW,pW);
    WireCell::GeomDataSource gds;
    gds.use_wires(wires);

    int ntot = gds.get_wires().size();
    cerr << "#wires = " << ntot << endl;
    assert(ntot == 32);

    int want_nwires[] = {11, 11, 10};

    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t plane = (WirePlaneType_t)iplane;
	GeomWireSelection wip = gds.wires_in_plane(plane);
	for (int iaxis=0; iaxis<3; ++iaxis) {
	    std::pair<float, float> mm = gds.minmax(iaxis, plane);
	    cerr << "axis " << iaxis << ": " << mm.first << " " << mm.second << endl;
	}

	cerr << "plane #" << iplane << " #wires=" << wip.size() << endl;
	assert(want_nwires[iplane] == wip.size());
	for (int wind=0; wind<wip.size(); ++wind) {
	    const GeomWire* wire = wip[wind];
	    cerr << "plane #" << iplane << " wire #" << wind
		 << " " << wire->point1() << " --> " << wire->point2()
		 << endl;
	}
    }

    return 0;
}
