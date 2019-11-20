#include "WCPNav/ExampleGDS.h"
#include "WCPNav/ParamGDS.h"

#include <iostream>
using namespace std;
using namespace WCP;

WCP::GeomDataSource* WCP::make_example_gds(float pitch, float angle,
						     float yextent, float zextent)
{
    float dx=10*units::mm;
    float dy=yextent;
    float dz=zextent;

    float angU = angle;
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

    return new WCP::ParamGDS(bmin,bmax, oU,pU, oV,pV, oW,pW);
}    


