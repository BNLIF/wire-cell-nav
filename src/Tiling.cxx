#include "WireCellNav/Tiling.h"
#include "WireCellUtil/IndexedSet.h"
#include "WireCellUtil/NamedFactory.h"

#include <boost/graph/adjacency_list.hpp>
#include <vector>

using namespace WireCell;

WIRECELL_NAMEDFACTORY(Tiling);
WIRECELL_NAMEDFACTORY_ASSOCIATE(Tiling, ICellSink);
WIRECELL_NAMEDFACTORY_ASSOCIATE(Tiling, ITiling);



Tiling::Tiling()
//    : m_graph(0)
{
}

Tiling::~Tiling()
{
    this->clear();
}


void Tiling::clear()
{
    // if (m_graph) {
    // 	delete m_graph;
    // 	m_graph = 0;
    // }
}

// fixme: actually implement some day....
void Tiling::sink(const ICell::iterator_range& cells) {}
IWire::iterator_range Tiling::wires(const ICell::pointer& cell) const {}
ICell::iterator_range Tiling::cells(const IWire::pointer& wire) const {}
ICell::iterator_range Tiling::cell(const IWire::iterator_range& wires) const {}
ICell::iterator_range Tiling::neighbors(const ICell::pointer& cell) const {}



#if 0				// fixme: reenable

/// Each vertex of the graph has a "type" and an index into a
/// vector of objects of corresponding type.:
/// - cell indexes into an array of Cell
/// - wire indexes into an array of Wire
/// - point index into an array of Point 
struct Property {
    enum VertexType_t { unknown=0, cell, wire, point};
    VertexType_t vtype;
    int index;
    Property(VertexType_t vt=unknown, int ind=-1) : vtype(vt), index(ind) {}
    
    bool operator<(const Property& rhs) const {
	if (vtype < rhs.vtype) return true;
	return index < rhs.index;
    }
    
};

/// The edges are implicitly of these four types:
/// - wirecell :: cell <--> wire
/// - neighbor :: cell <--> cell
/// - boundary :: point <--> point
/// - internal :: cell <--> point

typedef boost::adjacency_list<boost::setS, boost::vecS, 
			      boost::undirectedS,
			      Property > GraphType;	
typedef boost::graph_traits<GraphType>::vertex_descriptor Vertex;
typedef boost::graph_traits<GraphType>::edge_descriptor Edge;


// Hold on to the actual graph so we can hide this type from the header
namespace WireCell {
struct Graph {
    
    GraphType m_graph;

    Graph() {}

    /// BGL does not appear to support lookups by property, so DIY. 
    mutable std::map<Property, Vertex> cellVertex, wireVertex;
    Vertex cell_vertex(WireCell::Cell cell)  {
	Property prop = cell_property(cell);
	return cellVertex[prop];
    }
    Vertex wire_vertex(WireCell::Wire wire)  {
	Property prop = wire_property(wire);
	return wireVertex[prop];
    }
    
    /// A graph vertex may be a wire a cell or a point.  The graph
    /// stores this as a type and a key into an index for that type.
    /// For Wire and Cell, they also carry a unique ident() but that
    /// is ignored.
    WireCell::IndexedSet<WireCell::Point> point_index;
    WireCell::IndexedSet<WireCell::Cell>   cell_index;
    WireCell::IndexedSet<WireCell::Wire>   wire_index;
    
    Property point_property(const WireCell::Point& point)  {
	return Property(Property::point, point_index(point));
    }
    Property cell_property(WireCell::Cell cell)  {
	return Property(Property::cell, cell_index(cell));
    }
    Property wire_property(WireCell::Wire wire)  {
	return Property(Property::wire, wire_index(wire));
    }


    void record(WireCell::Cell thecell);
    WireCell::CellVector cells(WireCell::Wire wire);

};
}
    
WireCell::CellVector WireCell::Graph::cells(WireCell::Wire wire)
{
    Vertex vwire = wire_vertex(wire);

    WireCell::CellVector res;
    auto verts = boost::adjacent_vertices(vwire, m_graph);
    for (auto vit = verts.first; vit != verts.second; ++vit) {
	Vertex other = *vit;
	if (m_graph[other].vtype == Property::cell) {
	    res.push_back(cell_index.collection[m_graph[other].index]);
	}
    }
    return res;
}


void WireCell::Graph::record(WireCell::Cell thecell) 
{
    Property cell_prop = cell_property(thecell);
    Vertex cv = boost::add_vertex(cell_prop, m_graph);
    cellVertex[cell_prop] = cv;

    // wire vertices and wire-cell edges
    const WireCell::WireVector uvw_wires = thecell->wires();
    for (int ind=0; ind<uvw_wires.size(); ++ind) {
	Property wire_prop = wire_property(uvw_wires[ind]);
	Vertex w = boost::add_vertex(wire_prop, m_graph);
	wireVertex[wire_prop] = w;
	boost::add_edge(cv, w, m_graph);
    }

    // corners
    const WireCell::PointVector pcell = thecell->corners();
    size_t npos = pcell.size();
    std::vector<Vertex> corner_vertices;

    // corner vertices and internal edges
    for (int ind=0; ind < npos; ++ind) {
	Property point_prop = point_property(pcell[ind]);
        Vertex v = boost::add_vertex(point_prop, m_graph);
	corner_vertices.push_back(v);

	// center to corner internal edges
	boost::add_edge(cv, v, m_graph);
    }

    // cell boundary edges
    for (int ind=0; ind<npos; ++ind) {
	boost::add_edge(corner_vertices[ind], corner_vertices[(ind+1)%npos], m_graph);
    }
    
    // done except nearest neighbors

}




void WireCell::WireCell::load(const WireCell::CellSet& cellset)
{
    this->clear();
    m_graph = new Graph();

    for (auto it = cellset.begin(); it != cellset.end(); ++it) {
	Cell thecell = *it;
	m_graph->record(thecell);
    }
}

WireCell::WireVector WireCell::WireCell::wires(WireCell::Cell cell) const
{
    return cell->wires();	// this one is easy
}

WireCell::CellVector WireCell::WireCell::cells(WireCell::Wire wire) const
{
    if (!m_graph) {
	return CellVector();
    }

    return m_graph->cells(wire);
}

WireCell::Cell WireCell::WireCell::cell(const WireCell::WireVector& wires) const
{
    return Cell();		// fixme
}

WireCell::CellVector WireCell::WireCell::neighbors(WireCell::Cell cell) const
{
    return CellVector();	// fixme
}

#endif	// fixme
