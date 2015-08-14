#include "WireCellIface/IGeometry.h"
#include "WireCellIface/IConfigurable.h"

#include "WireCellUtil/Testing.h"
#include "WireCellUtil/NamedFactory.h"
#include "WireCellUtil/TimeKeeper.h"
#include "WireCellUtil/MemUsage.h"

#include <iostream>

using namespace WireCell;
using namespace std;

int main()
{
    WIRECELL_NAMEDFACTORY_USE(ParamGeometry);


    TimeKeeper tk("test rio");
    MemUsage mu("test rio");

    auto geom = WireCell::Factory::lookup<IGeometry>("ParamGeometry");

    auto wires = geom->wires();
    auto tiling = geom->tiling();

    for (auto wire : wires->wires_range()) {
	//ICellVector cells = tiling->cells(wire);
	//cout << wire->ident() << " with " << cells.size() << endl;
	//cout << wire->ident() << endl;
    }
}
