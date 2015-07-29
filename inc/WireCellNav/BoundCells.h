#ifndef WIRECELLNAV_BOUNDCELLS
#define WIRECELLNAV_BOUNDCELLS

#include "WireCellIface/IWire.h"
#include "WireCellIface/ICell.h"

namespace WireCell {

    // internal
    class BoundCell;

    /** A provider of cells using the bounded wire algorithm. */
    class BoundCells :
	public IWireSink,
	public ICellSequence
    {

    public:
	
	BoundCells();
	virtual ~BoundCells();

	/// Generate my cells (IWireSink interface).
	void sink(wire_iterator begin, wire_iterator end);

	/// Return iterator to first cell provided.
	cell_iterator cells_begin() const;

	/// Return iterator to one past last cell provided.
	cell_iterator cells_end() const;

	size_t cells_size() const { return m_store.size(); }

    private:
	typedef std::vector<BoundCell*> BoundCellStore;
	BoundCellStore m_store;
    };
}

#endif
