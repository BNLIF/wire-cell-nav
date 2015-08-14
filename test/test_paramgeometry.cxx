#include "WireCellIface/IGeometry.h"
#include "WireCellIface/IConfigurable.h"
#include "WireCellUtil/NamedFactory.h"
#include "WireCellUtil/Testing.h"

#include <iostream>
using namespace std;

int main()
{
    WIRECELL_NAMEDFACTORY_USE(ParamGeometry);

    auto c = WireCell::Factory::lookup<WireCell::IConfigurable>("ParamGeometry");
    AssertMsg(c, "No configurable interface to ParamGeometry");

    auto g = WireCell::Factory::lookup<WireCell::IGeometry>("ParamGeometry");
    AssertMsg(g, "No geometry interface to ParamGeometry"); 

    AssertMsg(g->wires(), "No wires");
    AssertMsg(g->cells(), "No cells");
    AssertMsg(g->tiling(), "No tiling");
    AssertMsg(g->wire_summary(), "No wire summary");

    int ncells = boost::distance(g->cells()->cells_range());
    cout << ncells << endl;

    AssertMsg(ncells == 26029, "Number of cells generated changed");
}
