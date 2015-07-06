#include "WireCellNav/ExampleWires.h"
#include "WireCellNav/ParamWires.h"

#include <iostream>
using namespace std;
using namespace WireCell;

WireCell::IWireGeometry* WireCell::make_example_wires(float pitch, float angle,
						      float yextent, float zextent)
{
    float nudge = 0.5*pitch;
    float dx=10*units::mm;
    float dy=yextent;
    float dz=zextent;

    Vector bmin(-0.5*dx, -0.5*dy, -0.5*dz);
    Vector bmax(+0.5*dx, +0.5*dy, +0.5*dz);
    Vector drift(1,0,0);

    Vector oU(0.2*dx, -0.5*dy + nudge, -0.5*dz + nudge);
    Vector oV(0.3*dx, -0.5*dy + nudge, +0.5*dz - nudge);
    Vector oW(0.1*dx,           nudge, -0.5*dz + nudge);

    Vector pU(0, pitch*std::cos(+angle), pitch*std::sin(+angle));
    Vector pV(0, pitch*std::cos(-angle), pitch*std::sin(-angle));
    Vector pW(0, 0, pitch);

    return new WireCell::ParamWires(bmin,bmax,drift, oU,pU, oV,pV, oW,pW);
}    


