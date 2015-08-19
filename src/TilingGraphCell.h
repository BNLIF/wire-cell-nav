#include "TilingGraph.h"

		   // fixme: implement this if we want to provide our
		   // own wires and cells which are space-efficient,
		   // holding only a pointer back to the graph and an
		   // graph Vertex.  If this feature is implemented it
		   // will require users to drop the wires and cells
		   // that were given as input as mixing the two
		   // families will likely lead to confusion and
		   // negate any space-savings that this feature would
		   // allow.       For now, just do things simply


namespace WireCell {
    
    class TilingGraphCell : public WireCell::ICell {
	TilingGraph::Vertex m_vertex;
    public:
        virtual int ident() const;
        virtual double area() const;
        virtual WireCell::Point center() const;
        virtual WireCell::PointVector corners() const;
	virtual WireCell::IWireVector wires() const;
    };
}
