#include "WireCellNav/Tiling.h"
#include "WireCellUtil/NamedFactory.h"

#include "TilingGraph.h"

#include <vector>
#include <iostream>

using namespace WireCell;

WIRECELL_NAMEDFACTORY(Tiling);
WIRECELL_NAMEDFACTORY_ASSOCIATE(Tiling, ICellSink);
WIRECELL_NAMEDFACTORY_ASSOCIATE(Tiling, IWireSummaryClient);
WIRECELL_NAMEDFACTORY_ASSOCIATE(Tiling, ITiling);


Tiling::Tiling()
    : m_graph(0)
    , m_cells()
{
}

Tiling::~Tiling()
{
    if (m_graph) {
    	delete m_graph;
    	m_graph = 0;
    }
}



bool Tiling::make_graph() const
{
    if (m_graph) {
	return true;
    }
    if (!m_ws) {
	std::cerr << "Tiling: not wire summary set" << std::endl;
	return false;
    }
    if (m_cells.begin() == m_cells.end()) {
	std::cerr << "Tiling: no cells set" << std::endl;
	return false;
    }

    m_graph = new TilingGraph(m_ws);
    for (auto cp : m_cells) {
	m_graph->record(cp);
    }
    return true;
}



IWireVector Tiling::wires(ICell::pointer cell) const 
{
    if (!make_graph()) {
	IWireVector v;
	return v;
    }
    return m_graph->wires(cell);
}
ICellVector Tiling::cells(IWire::pointer wire) const 
{
    if (!make_graph()) {
	ICellVector c;
	return c;
    }
    return m_graph->cells(wire);
}
ICell::pointer Tiling::cell(const IWireVector& wires) const 
{
    return m_graph->cell(wires);
}
ICellVector Tiling::neighbors(ICell::pointer cell) const 
{
    ICellVector v;
    return v;
}



    
