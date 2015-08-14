// this is a private header for Tiling.cxx.
// fixme: it's in the global namespace.

#ifndef WIRECELL_TILINGGRAPH
#define WIRECELL_TILINGGRAPH

#include "WireCellUtil/IndexedSet.h"

#include <boost/graph/adjacency_list.hpp>





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
	if (vtype == rhs.vtype) {
	    return index < rhs.index;
	}
	return vtype < rhs.vtype;
    }
    
};

/// The edges are implicitly of these four types:
/// - wirecell :: cell <--> wire
/// - neighbor :: cell <--> cell
/// - boundary :: point <--> point
/// - internal :: cell <--> point

typedef boost::adjacency_list<boost::setS, boost::vecS, 
			      boost::undirectedS,
			      Property > BoostTilingGraph;	
typedef boost::graph_traits<BoostTilingGraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<BoostTilingGraph>::edge_descriptor Edge;


namespace WireCell {

class TilingGraph {
    
public:

    typedef std::pair<float,float> Point2D;

    BoostTilingGraph m_graph;
    IWireSummary::pointer m_ws;

    TilingGraph(IWireSummary::pointer ws) : m_ws(ws) {}

    /// BGL does not appear to support lookups by property, so DI. 
    mutable std::map<Property, Vertex> cellVertex, wireVertex;
    Vertex cell_vertex(ICell::pointer cell)  {
	Property prop = cell_property(cell);
	return cellVertex[prop];
    }
    Vertex wire_vertex(IWire::pointer wire)  {
	Property prop = wire_property(wire);
	return wireVertex[prop];
    }
    
    /// A graph vertex may be a wire a cell or a point.  The graph
    /// stores this as a type and a key into an index for that type.
    /// For Wire and Cell, they also carry a unique ident() but that
    /// is ignored.
    WireCell::IndexedSet<Point2D>        point_index;
    WireCell::IndexedSet<ICell::pointer> cell_index;
    WireCell::IndexedSet<IWire::pointer> wire_index;
    
    Property point_property(const Point2D& point)  {
	return Property(Property::point, point_index(point));
    }
    Property cell_property(ICell::pointer cell)  {
	return Property(Property::cell, cell_index(cell));
    }
    Property wire_property(IWire::pointer wire)  {
	return Property(Property::wire, wire_index(wire));
    }


    void record(ICell::pointer thecell);

    /// ITiling helpers
    IWireVector wires(ICell::pointer cell);
    ICellVector cells(IWire::pointer wire);

};
    
} // namespace WireCell



// WireCell::IWireVector WireCell::TilingGraph::wires(WireCell::ICell::pointer cell)
// {
// }


static WireCell::IWireVector three_wires(WireCell::IWireSummary::pointer ws,
					 WireCell::ICell::pointer cell)
{
    using namespace WireCell;

    const Point center = cell->center();
    IWireVector thewires;
    WirePlaneType_t planes[3] = { kUwire, kVwire, kWwire };
    for (int ind=0; ind<3; ++ind) {
	IWire::pointer w = ws->closest(center, planes[ind]);
	if (ws) { 
	    thewires.push_back(w);
	}
    }
    return thewires;
}
					 
static WireCell::TilingGraph::Point2D threetotwo(const WireCell::Point& p3)
{
    return WireCell::TilingGraph::Point2D(p3.z(), p3.y());
}


void WireCell::TilingGraph::record(WireCell::ICell::pointer thecell) 
{
    using namespace WireCell;
    using namespace std;

    Property cell_prop = cell_property(thecell);
    Vertex cv = boost::add_vertex(cell_prop, m_graph);
    cellVertex[cell_prop] = cv;


    // wire vertices and wire-cell edges
    const IWireVector uvw_wires = three_wires(m_ws, thecell);
    if (uvw_wires.size() != 3) {
	cerr << "Found " << uvw_wires.size() << " wires " << thecell->center() << endl;
    }
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
	Property point_prop = point_property(threetotwo(pcell[ind]));
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


// return cells associated with wire
WireCell::ICellVector WireCell::TilingGraph::cells(WireCell::IWire::pointer wire)
{
    Vertex vwire = wire_vertex(wire);

    WireCell::ICellVector res;
    auto verts = boost::adjacent_vertices(vwire, m_graph);
    for (auto vit = verts.first; vit != verts.second; ++vit) {
	Vertex other = *vit;
	if (m_graph[other].vtype == Property::cell) {
	    res.push_back(cell_index.collection[m_graph[other].index]);
	}
    }
    return res;
}



// return wires associated with cell
WireCell::IWireVector WireCell::TilingGraph::wires(WireCell::ICell::pointer cell)
{
    Vertex vcell = cell_vertex(cell);

    WireCell::IWireVector res;
    auto verts = boost::adjacent_vertices(vcell, m_graph);
    for (auto vit = verts.first; vit != verts.second; ++vit) {
	Vertex other = *vit;
	if (m_graph[other].vtype == Property::wire) {
	    res.push_back(wire_index.collection[m_graph[other].index]);
	}
    }
    return res;
}







#endif
