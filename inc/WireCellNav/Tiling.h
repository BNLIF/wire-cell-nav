#ifndef WIRECELLNAV_WIRECELL_H
#define WIRECELLNAV_WIRECELL_H

#include "WireCellIface/ICell.h"
#include "WireCellIface/ITiling.h"


namespace WireCell {


    // Considered opaque.
    class TilingGraph;

    /** The reference implementation of WireCell::ITiling.
     *
     * The tiling answers questions about how the given cells are
     * tiled with respect to each other and the their associated wires.
     *
     */
    class Tiling : public ICellSink, public ITiling
		 , public IWireSequence, public ICellSequence
    {
    public:
	Tiling();
	virtual ~Tiling();

	/// Provide cells used to build the tiling.  Note, these cells
	/// are not retained by the tiling.  See the iterator methods
	/// below.
	virtual void sink(const ICell::iterator_range& cells);

	/// Provide access to a new family of cells corresponding to
	/// but differing from the input cells.  They are defined in a
	/// way to minimize memory usage but depend on this Tiling
	/// object being alive.
	virtual cell_iterator cells_begin();
	virtual cell_iterator cells_end();

	/// Provide access to the family of wires associated to the
	/// family of cells from the above iterators.  These
	/// correspond to but are different from the wires associated
	/// with the original "sunk" cells.
	virtual wire_iterator wires_begin();
	virtual wire_iterator wires_end();


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


    private:
	
	mutable TilingGraph* m_graph;
    };

} // namespace WireCell
#endif
