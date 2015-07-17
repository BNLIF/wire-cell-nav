#ifndef WIRECELLNAV_BOUNDCELLS
#define WIRECELLNAV_BOUNDCELLS

#include "WireCellIface/ICellGenerator.h"
#include "WireCellIface/ICellProvider.h"

namespace WireCell {

    /** A provider of cells using the bounded wire algorithm. */
    class BoundCells :
	public ICellGenerator,
	public ICellProvider
    {

    public:
	
	BoundCells();
	virtual ~BoundCells();

	/// Generate my cells (ICellGenerator interface).
	void generate(const WireCell::IWireDatabase& wdb);

	/// Lend access to the cells provided (ICellProvider interface).
	const WireCell::CellSet& cells() const;

    private:
	WireCell::CellSet m_store;
    };
}

#endif
