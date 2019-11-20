#ifndef WIRECELLNAV_PARAMGEOMDATASOURCE_H
#define WIRECELLNAV_PARAMGEOMDATASOURCE_H

#include "WCPNav/GeomDataSource.h"
#include "WCPData/Vector.h"
#include "WCPData/GeomWire.h"

#include <map>
#include <vector>

namespace WCP {

    /**
       A source of wire geometry generated from parameters.

     */
    class ParamGDS : public WCP::GeomDataSource {
    public:

	/** Create a parameterized geometry data source.
	 *
	 * This class defines three planes of wires.  
	 *
	 * Within one plane, all wires are parallel and separated by a
	 * uniform pitch and the planes are transverse to the given
	 * "drift" direction which is taken to be in the -X direction.
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
	ParamGDS(const Vector& minbound, const Vector& maxbound, 
		 const Vector& offsetU, const Vector& pitchU,
		 const Vector& offsetV, const Vector& pitchV,
		 const Vector& offsetY, const Vector& pitchY);
	virtual ~ParamGDS();

    private:
	int make_wire_plane(const Vector& offset, const Vector& pitch,
			    WirePlaneType_t plane);

	bool make_wire(std::vector<WCP::GeomWire>& all_wires,
		       int index, const Vector& point,
		       const Vector& proto);

	Vector minbound, maxbound;
    };

}

#endif
