#ifndef WIRECELLNAV_PARAMWIRES_H
#define WIRECELLNAV_PARAMWIRES_H

#include "WireCellIface/IWireProvider.h"
#include "WireCellIface/IConfigurable.h"
#include "WireCellUtil/Units.h"

#include <map>

namespace WireCell {

    /** A provider of wire (segment) geometry as generated from parameters.
     *
     * All wires in one plane are constructed to be parallel to
     * one-another and to be equally spaced between neighbors and
     * perpendicular to the drift direction.
     */
    class ParamWires : public IWireProvider, public IConfigurable {
    public:
	ParamWires();
	virtual ~ParamWires();

	/** Generate the parameterized wire geometry.
	 *
	 * Three planes of wires are generated and labeled, U, V and
	 * W.  There is no internal convention as to which plane is
	 * which except that the label indicates the direction that
	 * their wires point (and not their transverse pitch
	 * direction).
	 *
	 * The coordinate conventions are defined as:
	 * 
	 * - wire planes are parallel to the Y-Z plane.
	 *
	 * - electrons drift in the negative-X direction.
	 *
	 * - wire angles are measured in the Y-Z plane w.r.t. the
	 *   direction of the Y-axis.
	 *
	 * - cross-product of the direction of a plane's wires to the
	 *   direction of the plane's pitch is the direction of drift
	 *   (WxP=D).
	 *
	 * Thus wire angles
	 *
	 * - (0,90)  : wires point in +Y,-Z direction, pitch in +Y,+Z
	 * - (90,180): wires point in -Y,-Z direction, pitch in +Y,-Z
	 * 
	 * The wires planes are fully specified by four rays.
	 *
	 * \param bounds is a WireCell::Ray with its tail at the
	 * negative-most corner and its head at the positive-most
	 * corner of a rectangular bounding box.  All wires will be
	 * constructed so that they terminate on the walls of this
	 * box.
	 *
	 * \param U is a WireCell::Ray which is in the wire plane and
	 * points in a direction perpendicular to the wire direction.
	 * Both tail and head points should be inside the bounding box
	 * and on wires.
	 *
	 * \param V is same but same for V wire plane.
	 *
	 * \param W is same but for for W wire plane.
	 *
	 * Note, this class does not implement wire wrapping.  Wire
	 * and channel ID numbers are equated.
	 *
	 */
	void generate(const Ray& bounds,
		      const Ray& U, const Ray& V, const Ray& W);


	/** Generate the parameterized wire geometry.
	 *
	 * This method gives a simpler, more restricted interface to
	 * generating the wires.  It follows the same conventions as
	 * the one above but assumes the bounding box is centered on
	 * the origin, assumes U points in the +Y,-Z, V points in the
	 * -Y,-Z and W points in the +Y(Z=0) directions.  U is at
	 * +0.25*dx, W is at -0.25*dx and V is at x=0.  It is assumed
	 * that one wire of every plane goes through Y=0,Z=0.  It is
	 * assumed all planes have equal pitch.
	 */
	void generate(float dx=10*units::mm,
		      float dy=1*units::meter,
		      float dz=1*units::meter,
		      float pitch = 10*units::mm,
		      float angle = 60.0*units::degree);

	/** Configurable interface.
	 */
	virtual void configure(const WireCell::Configuration& config);
	virtual WireCell::Configuration default_configuration() const;


	/// Lend access to the store of produced wires.  IWireProvider
	/// interface.
	const WireStore& wires() const;


    private:

	void clear();
	WireStore m_wire_store;

    };


}

#endif
