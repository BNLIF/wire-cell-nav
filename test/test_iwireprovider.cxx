#include "WireCellIface/IWireProvider.h"
#include "WireCellIface/IConfigurable.h"
#include "WireCellUtil/Testing.h"
#include "WireCellUtil/NamedFactory.h"

#include <iostream>

using namespace WireCell;
using namespace std;


int main()
{
    WIRECELL_NAMEDFACTORY_USE(ParamWires);

    auto wp = WireCell::Factory::lookup<IWireProvider>("ParamWires");
    Assert(wp, "Failed to get IWireProvider from default ParamWires");
    cout << "Got ParamWires IWireProvider interface @ " << wp << endl;

    // fixme: this needs to be done by a configuration service
    auto wp_cfg = WireCell::Factory::lookup<IConfigurable>("ParamWires");
    Assert(wp_cfg, "Failed to get IConfigurable from default ParamWires");
    wp_cfg->configure(wp_cfg->default_configuration());
    cout << "Got ParamWires IConfigurable interface @ " << wp << endl;

    int nwires = wp->wires().size();
    cout << "Got " << nwires << " wires" << endl;
    Assert(1103 == nwires);
    return 0;
}

