#ifndef WIRECELLNAV_CELLGRAPH
#define WIRECELLNAV_CELLGRAPH

#include "WireCellIface/ICellDatabase.h"

namespace WireCell {


    struct Graph;

    /** A cell database that stores the tiling as a graph. */
    class CellDatabase : public ICellDatabase
    {
    public:
	CellDatabase();
	virtual ~CellDatabase();

	/// Load the underling cell data.
	virtual void load(cell_range cells);

	/// Return collection of wires which are associated with the
	/// given cell.
	virtual wire_range wires(Cell cell) const;
	
	/// Return collection of cells associated with the given wire.
	virtual cell_range cells(Wire wire) const;

	/// Return the one cell associated with the collection of
	/// wires or NULL.
	virtual Cell cell(wire_range wires) const;

	/// Return collection of nearest neighbor cells.
	virtual cell_range neighbors(Cell cell) const;


    private:

	void clear();

//	Graph* m_graph;
    };

} // namespace WireCell
#endif
