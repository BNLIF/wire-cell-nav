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

	/// Load the underling cell data. (ICellDatabase)
	virtual void load(const WireCell::CellSet& cells);

	/// Return collection of wires which are associated with the
	/// given cell. (ICellDatabase)
	virtual WireVector wires(Cell cell) const;
	
	/// Return collection of cells associated with the given
	/// wire. (ICellDatabase)
	virtual CellVector cells(Wire wire) const;

	/// Return the one cell associated with the collection of
	/// wires or NULL. (ICellDatabase)
	virtual Cell cell(const WireVector& wires) const;

	/// Return collection of nearest neighbor
	/// cells. (ICellDatabase)
	virtual CellVector neighbors(Cell cell) const;

    private:

	void clear();

	Graph* m_graph;
    };

} // namespace WireCell
#endif
