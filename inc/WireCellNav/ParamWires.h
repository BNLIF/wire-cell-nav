#ifndef WIRECELLNAV_PARAMWIRES_H
#define WIRECELLNAV_PARAMWIRES_H

#include "WireCellIface/IWireGeometry.h"
#include "WireCellData/Vector.h"

#include <map>

namespace WireCell {

    /**
       A source of wire geometry generated from parameters.

     */
    class ParamWires : public WireCell::IWireGeometry {
    public:

	/** Create a parameterized wire geometry.
	 *
	 * This class defines three planes of wires.  
	 *
	 * Within one plane, all wires are parallel and separated by a
	 * uniform pitch and the planes are transverse to the given
	 * "drift" direction.
	 *
	 * The wire planes will be bounded by a rectangular solid
	 * defined by its two diagonally apposed corners.  These
	 * points determine the coordinate system of the resulting
	 * wires.
	 *
	 * Each wire plane is defined by two 3-vectors. The "offset"
	 * vector is interpreted to point from the minimum boundary
	 * point to a point on the first wire of the view.  The
	 * "pitch" vector is interpreted to run perpendicular to the
	 * wires in the plane and with a magnitude equal to the pitch
	 * between the wires.
	 *
	 * Note, this class does not implement wire wrapping.  Wire
	 * and channel ID numbers are equated.
	 */
	ParamWires(const Vector& minbound, const Vector& maxbound, const Vector& drift,
		   const Vector& offsetU, const Vector& pitchU,
		   const Vector& offsetV, const Vector& pitchV,
		   const Vector& offsetY, const Vector& pitchY);
	virtual ~ParamWires();

	const WireCell::GeomWireSet& get_wires() const;


    private:
	int make_wire_plane(const Vector& offset, const Vector& pitch,
			    WirePlaneType_t plane) const;
	bool inside(const Vector& point) const;

	mutable WireCell::GeomWireSet wires;
	Vector minbound, maxbound, drift;
    };

}

#endif
