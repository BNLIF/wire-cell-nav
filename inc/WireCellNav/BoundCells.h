#ifndef WIRECELLNAV_BOUNDCELLS
#define WIRECELLNAV_BOUNDCELLS

#include "WireCellIface/ICellGenerator.h"
#include "WireCellIface/ICellProvider.h"

namespace WireCell {

    // internal
    class BoundCell;

    /** A provider of cells using the bounded wire algorithm. */
    class BoundCells :
	public ICellGenerator,
	public ICellProvider
    {

    public:
	
	BoundCells();
	virtual ~BoundCells();

	/// Generate my cells (ICellGenerator interface).
	void generate(wire_iterator begin, wire_iterator end);

	/// Return iterator to first cell provided.
	cell_iterator cells_begin();

	/// Return iterator to one past last cell provided.
	cell_iterator cells_end();


    private:
	typedef std::vector<BoundCell*> BoundCellStore;
	BoundCellStore m_store;
    };
}

#endif
