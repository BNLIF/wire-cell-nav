#include "WireCellNav/ExampleGDS.h"
#include "WireCellNav/GenerativeFDS.h"
#include "WireCellNav/PepperDepositor.h"

#include "WireCellData/Units.h"

#include <iostream>


int main () {
    using namespace std;
    const float width = 1*units::m;
    const WireCell::PepperDepositor::MinMax side(-width,width);
    const WireCell::PepperDepositor::MinMax charge(1,100);
    WireCell::PepperDepositor dep(side, side, charge);
    
    WireCell::GeomDataSource* gds = WireCell::make_example_gds();    

    WireCell::GenerativeFDS fds(dep, *gds);

    for (int ind=0; ind<10; ++ind) {
	int frame = fds.jump(ind);
	cout << "frame #" << frame << " size:" << fds.size() << endl;
    }

    return 0;
}
