#ifndef WIRECELLNAV_PEPPERDEPOSITOR
#define WIRECELLNAV_PEPPERDEPOSITOR

#include "WCPNav/Depositor.h"

#include <map>

namespace WCP {

    /** A Depositor which simply peppers a volume covered by the wires
     * with random hits
     */
    class PepperDepositor : public Depositor
    {
    public:
	typedef std::pair<float,float> MinMax;

	PepperDepositor(MinMax xmm, MinMax ymm, MinMax zmm, MinMax qmm,
			int hits_per_frame = 20);
	virtual ~PepperDepositor();

	const PointValueVector& depositions(int frame_number) const;

    private:
	MinMax xmm, ymm, zmm, qmm;
	int hits_per_frame;

	// cache this long enough so the depositions() return can be used w/out copy
	// make it mutable so we can otherwise be const.
	mutable PointValueVector hits;
    };


}

#endif
