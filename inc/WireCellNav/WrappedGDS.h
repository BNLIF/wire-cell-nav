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
	 * \param angle gives the absolute value of the U and V angles
	 * w.r.t. the W (aka Y) wires..
	 *
	 * \param pitch gives the perpendicular distance between any
	 * two consecutive wires in a plane.
	 * 
	 */
    	WrappedGDS(const Vector& minbound, const Vector& maxbound, 
		   double angle, double pitch);
	WrappedGDS(const Vector& minbound, const Vector& maxbound,
		   double angleU, double angleV, double pitch,
		   short cryo, short apa);
    	virtual ~WrappedGDS();

	const Vector get_minbound() const {return m_minbound;}
	const Vector get_maxbound() const {return m_maxbound;}
	const short cryo() const {return _cryo;}
	const short apa() const {return _apa;}

    private:

    	Vector m_minbound, m_maxbound;
	short _cryo;
	short _apa;

	void uvw_wire_mesh(double angle, double uvpitch);
	void uv_wire_mesh(double angle, double pitch, bool is_u);
	void w_wire_mesh(double pitch);
    };
    

}

#endif
