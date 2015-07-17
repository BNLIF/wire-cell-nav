#ifndef WIRECELLNAV_PARAMWIRES_H
#define WIRECELLNAV_PARAMWIRES_H

#include "WireCellIface/IWireProvider.h"
#include "WireCellIface/IWireGenerator.h"

#include <map>

namespace WireCell {

    /** A provider of wire (segment) geometry as generated from parameters.
     *
     * All wires in one plane are constructed to be parallel to
     * one-another and to be equally spaced between neighbors and
     * perpendicular to the drift direction.
     */
    class ParamWires :
	public IWireGenerator,
	public IWireProvider {
    public:
	ParamWires();
	virtual ~ParamWires();

	void generate(const WireCell::IWireParameters& params);

	/// Lend access to the store of produced wires.  IWireProvider
	/// interface.
	const WireSet& wires() const;


    private:

	void clear();
	WireSet m_wire_store;


    };


}

#endif
