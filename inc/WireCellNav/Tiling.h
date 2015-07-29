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
	virtual void sink(cell_iterator begin, cell_iterator end);


	/// Return a sequence of wires which are associated with the
	/// given cell.
	virtual wire_range wires(const ICell& cell) const;
	
	/// Return a sequence of cells associated with the given wire.
	virtual cell_range cells(const IWire& wire) const;

	/// Return the cells associated with the collection of wires.
	/// Typically this is expected to be one wire from each plane.
	virtual cell_range cell(const std::vector<const IWire*>& wires) const;

	/// Return collection of nearest neighbor cells.
	virtual cell_range neighbors(const ICell& cell) const;



    private:

	void clear();

//	Graph* m_graph;
    };

} // namespace WireCell
#endif
