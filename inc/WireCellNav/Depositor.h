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
	 * 
	 * Point.x : the time, relative to 0 and in units of time bins
	 *
	 * Point.y/z : the vertical/horizontal positions transverse to
	 * the drift direction.
	 *
	 * Value : the "charge" that the "hit" deposited.
	 *
	 * Note: there is no guarantee that frame_number is
	 * monotonically increasing from call to call.
	 * 
	 */
	virtual const PointValueVector& depositions(int frame_number) const = 0;
    };

}

#endif
