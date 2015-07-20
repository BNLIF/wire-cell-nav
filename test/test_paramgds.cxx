#include "WireCellNav/ParamGDS.h"
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
    float pitch = 10.0*units::mm;

    float angU = 60.0*units::pi/180.0;
    float angV = M_PI-angU;
    float angW = 0.0;

    const Vector center;
    const Vector deltabb(dx,dy,dz);
    const Vector bmin = center - 0.5*deltabb;
    const Vector bmax = center + 0.5*deltabb;

    // pitch vectors
    const Vector pU(0, pitch*std::cos(+angU), pitch*std::sin(+angU));
    const Vector pV(0, pitch*std::cos(+angV), pitch*std::sin(+angV));
    const Vector pW(0, pitch*std::cos(+angW), pitch*std::sin(+angW));

    // offset/starting points
    const Vector oU(+0.25*dx, 0.0, 0.0);
    const Vector oV( 0.0    , 0.0, 0.0);
    const Vector oW(-0.25*dx, 0.0, 0.0);

    cerr << "bmin=" << bmin << " bmax=" << bmax << endl;
    cerr << "Uoffset=" << oU << " Upitch=" << pU << endl;
    cerr << "Voffset=" << oV << " Vpitch=" << pV << endl;
    cerr << "Woffset=" << oW << " Wpitch=" << pW << endl;

    ParamGDS gds(bmin,bmax, oU,pU, oV,pV, oW,pW);
    int ntot = gds.get_wires().size();
    cerr << "#wires = " << ntot << endl;
    //assert(ntot == 32);


    int want_nwires[] = {135, 135, 101};

    for (int iplane=0; iplane<3; ++iplane) {
	WirePlaneType_t plane = (WirePlaneType_t)iplane;
	GeomWireSelection wip = gds.wires_in_plane(plane);
	cerr << "Plane " << iplane << " with " << wip.size() << " wires" << endl;
	assert(wip.size());
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
