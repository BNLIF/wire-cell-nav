#include "WireCellNav/Tiling.h"
#include "WireCellUtil/NamedFactory.h"

#include "TilingGraph.h"

#include <vector>

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
    if (m_graph) return true;
    if (!m_ws) return false;
    if (m_cells.begin() == m_cells.end()) return false;

    m_graph = new TilingGraph(m_ws);
    for (auto cp : m_cells) {
	m_graph->record(cp);
    }
}


IWireVector Tiling::wires(const ICell::pointer& cell) const {
    if (!make_graph()) {
	return IWireVector();
    }
    return m_graph->wires(cell);
}
ICellVector Tiling::cells(const IWire::pointer& wire) const {}
ICellVector Tiling::cell(const IWireVector& wires) const {}
ICellVector Tiling::neighbors(const ICell::pointer& cell) const {}



    
