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
    
    class TilingGraphWire : public WireCell::IWire {
	TilingGraph::Vertex m_vertex;
    public:
	virtual int ident() const;
	virtual WirePlaneType_t plane() const;
	virtual int index() const;
	virtual int channel() const;
	virtual int segment() const;
	virtual int face() const;
	virtual int apa() const;
	virtual WireCell::Ray ray() const;
    };
}
