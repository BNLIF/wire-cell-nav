#ifndef WIRECELLNAV_PEPPERDEPOSITOR
#define WIRECELLNAV_PEPPERDEPOSITOR

#include "WireCellNav/Depositor.h"

#include <map>

namespace WireCell {

    /** A Depositor which simply peppers a volume covered by the wires
     * with random hits
     */
    class PepperDepositor : public Depositor
    {
    public:
	typedef std::pair<float,float> MinMax;

	PepperDepositor(MinMax ymm, MinMax zmm, MinMax qmm, int hits_per_frame = 20);
	virtual ~PepperDepositor();

	const PointValueVector& depositions(int frame_number, int bins_per_frame) const;

    private:
	int hits_per_frame;
	MinMax ymm, zmm, qmm;

	// cache this long enough so the depositions() return can be used w/out copy
	// make it mutable so we can otherwise be const.
	mutable PointValueVector hits;
    };


}

#endif
