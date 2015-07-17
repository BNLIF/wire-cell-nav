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
	virtual void generate(const WireCell::IWireDatabasePtr wdb);

	/// Lend access to the cells provided (ICellProvider interface).
	virtual const WireCell::CellSet& cells() const;

    private:
	WireCell::CellSet m_store;
    };
}

#endif
