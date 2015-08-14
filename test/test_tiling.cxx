#include "WireCellIface/IWireParameters.h"
#include "WireCellIface/IWireGenerator.h"
#include "WireCellIface/IWire.h"
#include "WireCellIface/ICell.h"
#include "WireCellIface/ITiling.h"
#include "WireCellIface/IConfigurable.h"

#include "WireCellUtil/Testing.h"
#include "WireCellUtil/NamedFactory.h"
#include "WireCellUtil/TimeKeeper.h"
#include "WireCellUtil/MemUsage.h"

using namespace WireCell;

int main()
{
    WIRECELL_NAMEDFACTORY_USE(WireParams);
    WIRECELL_NAMEDFACTORY_USE(ParamWires);
    WIRECELL_NAMEDFACTORY_USE(BoundCells);
    WIRECELL_NAMEDFACTORY_USE(Tiling);


    TimeKeeper tk("test rio");
    MemUsage mu("test rio");



}
