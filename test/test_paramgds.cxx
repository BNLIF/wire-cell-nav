#include "WireCellNav/ParamGDS.h"
#include "WireCellData/Units.h"


#include <iostream>
#include <cmath>

using namespace WireCell;
using namespace std;

int main()
{
    float nudge = 1.0*units::mm;
    float dx=10*units::mm;
    float dy=1*units::meter;
    float dz=1*units::meter;
    float pitch = 3.0*units::mm;
    float angle = 60.0*units::pi/180.0;

    Vector bmin(-0.5*dx, -0.5*dy, -0.5*dz);
    Vector bmax(+0.5*dx, +0.5*dy, +0.5*dz);
    Vector drift(1,0,0);

    Vector oY(0.1*dx, nudge, nudge);
    Vector oU(0.2*dx, nudge, nudge);
    Vector oV(0.3*dx, nudge, nudge);

    Vector pY(0, 0, pitch);
    Vector pU(0, pitch*std::cos(+angle), pitch*std::sin(+angle));
    Vector pV(0, pitch*std::cos(-angle), pitch*std::sin(-angle));

    ParamGDS gds(bmin,bmax,drift, oU,pU, oV,pV, oY,pY);
    
    cerr << "#wires = " << gds.get_wires().size() << endl;



    return 0;
}
