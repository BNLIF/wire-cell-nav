#ifndef WIRECELL_WIREPARAMS
#define WIRECELL_WIREPARAMS

#include "WireCellIface/IConfigurable.h"
#include "WireCellIface/IWireParameters.h"
#include "WireCellUtil/Units.h"

namespace WireCell {

    /** Embody parameters describing a triple of wire planes and provide a configurable interface. */
    class WireParams : 
	public IWireParameters,
	public IConfigurable  {
    public:
	/** Fundamentally, the wires are defined by four rays.
	 *
	 * Three planes of wires are labeled, U, V and W.  There is no
	 * internal convention as to which plane is which except that
	 * the label indicates the direction that their wires point
	 * (and not their transverse pitch direction).
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
	void set(const Ray& bounds,
		 const Ray& U, const Ray& V, const Ray& W);


	/** Set the wire parameters simply.
	 *
	 * This method gives a simpler, more restricted interface to
	 * setting the wire parameter.  It follows the same
	 * conventions as the one above but assumes the bounding box
	 * is centered on the origin, assumes U points in the +Y,-Z, V
	 * points in the -Y,-Z and W points in the +Y(Z=0) directions.
	 * U is at +0.25*dx, W is at -0.25*dx and V is at x=0.  It is
	 * assumed that one wire of every plane goes through Y=0,Z=0.
	 * It is assumed all planes have equal pitch.
	 */
	void set(float dx=10*units::mm,
		 float dy=1*units::meter,
		 float dz=1*units::meter,
		 float pitch = 10*units::mm,
		 float angle = 60.0*units::degree);

	/** Provide access to the rays which were used to define the wires. */
	const Ray& bounds() const;
	const Ray& pitchU() const;
	const Ray& pitchV() const;
	const Ray& pitchW() const;
	
	/** Configurable interface.
	 */
	virtual void configure(const WireCell::Configuration& config);
	virtual WireCell::Configuration default_configuration() const;

	WireParams();
	virtual ~WireParams();

    private:
	Ray m_bounds, m_pitchU, m_pitchV, m_pitchW;
    };
}

#endif
