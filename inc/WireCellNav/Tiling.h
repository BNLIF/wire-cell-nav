#ifndef WIRECELLNAV_WIRECELL_H
#define WIRECELLNAV_WIRECELL_H

#include "WireCellIface/ICell.h"
#include "WireCellIface/IWireSummary.h"
#include "WireCellIface/ITiling.h"


namespace WireCell {


    // Considered opaque.
    class TilingGraph;

    /** A default implementation of WireCell::ITiling.
     *
     */
    class Tiling : public ICellSink, public IWireSummaryClient, public ITiling
    {
    public:
	Tiling();
	virtual ~Tiling();

	/// Provide access to the collection of cells. (ICellSink)
	virtual void sink(const ICell::iterator_range& cells) {
	    m_cells = cells;
	};

	/// Get a wire summary. (IWireSummaryClient)
	virtual void set(IWireSummary::pointer ws) {
	    m_ws = ws;
	};

	/// The rest are ITiling interface methods.

	/// Return a sequence of wires which are associated with the
	/// given cell.
	virtual IWireVector wires(ICell::pointer cell) const;
	
	/// Return a sequence of cells associated with the given wire.
	virtual ICellVector cells(IWire::pointer wire) const;

	/// Return the cell associated with the collection of wires,
	/// if any.
	virtual ICell::pointer cell(const IWireVector& wires) const;

	/// Return collection of nearest neighbor cells.
	virtual ICellVector neighbors(ICell::pointer cell) const;


	/// Explicitly generate the graph.  This is triggered lazily
	/// by the ITiling interface methods and requires that cells
	/// and wire summary have been obtained.
	bool make_graph() const; // lazy

    private:

	IWireSummary::pointer m_ws;
	ICell::iterator_range m_cells;

	mutable TilingGraph* m_graph; // lazy
    };

} // namespace WireCell
#endif
