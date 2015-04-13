#ifndef WIRECELLNAV_DEPOSITOR
#define WIRECELLNAV_DEPOSITOR

#include "WireCellData/Point.h"

namespace WireCell {

    /**
     * A Depositor produces "hit" depositions in time.
     * 
     * This class is abstract and may be used as a mixin.
     *
     * See GenerativeFDS 
     */
    // in the future this may also be used for some kind of hit-cell generator.
    class Depositor {
    public:
	virtual ~Depositor();

	/**
	 * Return depositions not before the given time bin.
	 *
	 * Implementation should return a vector of point-value pairs.
	 * The value should be interpreted as an amount of "charge" a
	 * "hit" deposited.  The X direction of each point is expected
	 * to be in units of time bins from the start of the frame and
	 * the Y/Z are the usual vertical/horizontal dimensions
	 * perpendicular to the drift direction (in units of length).
	 *
	 * Any points with an X value outside of [0, bins_per_frame]
	 * may be ignored.
	 *
	 * Note: there is no guarantee on the ordering of frame_number
	 * that the method sees nor that the bins_per_frame is
	 * unchanged from call to call.
	 * 
	 */
	virtual const PointValueVector& depositions(int frame_number, int bins_per_frame) const = 0;
    };

}

#endif
