#include "WireCellNav/ExampleGDS.h"
#include "WireCellNav/ParamGDS.h"

using namespace WireCell;

WireCell::GeomDataSource* WireCell::make_example_gds(float pitch, float angle,
						     float yextent, float zextent)
{
    float nudge = 1.0*units::mm;
    float dx=10*units::mm;
    float dy=yextent;
    float dz=zextent;

    Vector bmin(-0.5*dx, -0.5*dy, -0.5*dz);
    Vector bmax(+0.5*dx, +0.5*dy, +0.5*dz);
    Vector drift(1,0,0);

    Vector oY(0.1*dx, nudge, nudge);  // 2
    Vector oU(0.2*dx, nudge, nudge);  // 0
    Vector oV(0.3*dx, 11, -11); // 1

    Vector pY(0, 0, pitch);
    Vector pU(0, pitch*std::cos(+angle), pitch*std::sin(+angle));
    Vector pV(0, pitch*std::cos(-angle), pitch*std::sin(-angle));

    return new WireCell::ParamGDS(bmin,bmax,drift, oU,pU, oV,pV, oY,pY);
}    


