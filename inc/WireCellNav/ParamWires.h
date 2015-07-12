#ifndef WIRECELLNAV_PARAMWIRES_H
#define WIRECELLNAV_PARAMWIRES_H

#include "WireCellIface/IWireProvider.h"
#include "WireCellUtil/Units.h"

#include <map>

namespace WireCell {

    /** A provider of wire (segment) geometry as generated from parameters.
     *
     * All wires in one place are constructed to be parallel to
     * one-another and to be equally spaced between neighbors and
     * perpendicular to the drift direction.
     */
    class ParamWires : public IWireProvider {
    public:

	/** Create the parameterized wire geometry.
	 *
	 * \param bounds a WireCell::Ray defining a rectangular
	 * bounding box for all three wire planes.
	 *
	 * \param stepU a WireCell::Ray whose tail point is on the
	 * first wire of the U wire plane and whose head point is on
	 * the second and whose direction is perpendicular to both.
	 *
	 * \param stepV same for V wire plane.
	 *
	 * \param stepW same for W (aka Y) wire plane.
	 *
	 * \param drift a WireCell::Point giving the direction in
	 * which electrons drift w.r.t. the wire bounding box
	 * coordinates.
	 *
	 * Note, this class does not implement wire wrapping.  Wire
	 * and channel ID numbers are equated.
	 */
	ParamWires(const Ray& bounds, 
		   const Ray& stepU, const Ray& stepV, const Ray& stepW, 
		   const Vector& drift = Vector(-1,0,0));

	virtual ~ParamWires();

	/// Access the store of produced wires.
	const WireStore& wires() const;


    private:

	WireStore m_wire_store;

    };

    /** Return a pararameterized wire provider based on simple
     * parameters.
     *
     * Caller takes ownership of the returned object.
     **/
    IWireProvider* make_paramwires(float dx=10*units::mm,
				   float dy=1*units::meter,
				   float dz=1*units::meter,
				   float pitch = 10*units::mm,
				   float angle = 60.0*units::degree);


}

#endif
