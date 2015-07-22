#ifndef WIRECELLNAV_WRAPPEDGEOMDATASOURCE_H
#define WIRECELLNAV_WRAPPEDGEOMDATASOURCE_H

#include "WireCellNav/GeomDataSource.h"
#include "WireCellData/Vector.h"
#include "WireCellData/GeomWire.h"

#include <map>
#include <vector>
#include <memory>

namespace WireCell {

    /**
       A parameterized source of wire geometry in a wrapped configuration.
     */
    class WrappedGDS : public WireCell::GeomDataSource {
    public:

	/** Create a wrapped wire geometry data source from parameters.
	 *
	 * This class defines six planes of wires, three on either
	 * face of an APA.
	 *
	 * The U and V planes on one face consist of wires (segments)
	 * which logically wrap around to the other face.  This
	 * wrapping is implicit in that multiple wires will have the
	 * same channel number.
	 *
	 * Numerology: 
	 *
	 * - face: wires are parametersized on face "A" and wires on
	 * face "B" follow.
	 *
	 * - channel:
	 *
	 * - ident:
	 *
	 * The geometry is constructed with the following parameters.
	 *
	 * \param minbound is a vector giving the minimum point of the
	 * bounding box of the wire planes.  Wires will be defined
	 * such that they do not extend beyond the bounding box. 
	 *
	 * \param maxbound is a vector giving the maxbound point of
	 * the bounding box.
	 *
	 * \param offsetU is a point on a wire in the U plane of face
	 * A.  
	 *
	 * \param pitchU is a vector perpendicular to and connecting
	 * two neighboring wires in the U plane of face A.
	 *
	 * \param offsetV offset vector for V wires on face A.
	 *
	 * \param pitchV pitch vector for V wires on face A.
	 * 
	 * \param offsetW offset vector for W wires on face A.
	 *
	 * \param pitchW pitch vector for W wires on face A.
	 * 
	 */
    	WrappedGDS(const Vector& minbound, const Vector& maxbound, 
		   double angle, double uvpitch, double wpitch = 0.0);
    	virtual ~WrappedGDS();

    private:

    	Vector m_minbound, m_maxbound;


	void uvw_wire_mesh(double angle, double uvpitch);

    };
    



}

#endif
