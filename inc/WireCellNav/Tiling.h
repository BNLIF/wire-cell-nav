#ifndef WIRECELLNAV_WIRECELL_H
#define WIRECELLNAV_WIRECELL_H

#include "WireCellIface/ICell.h"
#include "WireCellIface/ITiling.h"

namespace WireCell {


    // Uses Boost Graph Library under the hood.
    struct Graph;

    /** A default implementation of WireCell::ITiling.
     *
     */
    class Tiling : public ICellSink, public ITiling
    {
    public:
	Tiling();
	virtual ~Tiling();

	/// Load the underling cell data.
	virtual void sink(const ICell::iterator_range& cells);


	/// Return a sequence of wires which are associated with the
	/// given cell.
	virtual IWire::iterator_range wires(const ICell::pointer& cell) const;
	
	/// Return a sequence of cells associated with the given wire.
	virtual ICell::iterator_range cells(const IWire::pointer& wire) const;

	/// Return the cells associated with the collection of wires.
	/// Typically this is expected to be one wire from each plane.
	virtual ICell::iterator_range cell(const IWire::iterator_range& wires) const;

	/// Return collection of nearest neighbor cells.
	virtual ICell::iterator_range neighbors(const ICell::pointer& cell) const;



    private:

	void clear();

//	Graph* m_graph;
    };

} // namespace WireCell
#endif
