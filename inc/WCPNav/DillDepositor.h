#ifndef WIRECELLNAV_DILLDEPOSITOR
#define WIRECELLNAV_DILLDEPOSITOR

#include "WCPNav/Depositor.h"
#include "WCPData/Units.h"

#include <map>

namespace WCP {

    /** A Depositor which sprays little dill seeds through the volume.
     */
    class DillDepositor : public Depositor
    {
    public:
	typedef std::pair<float,float> MinMax;

	DillDepositor(MinMax xmm, MinMax ymm, MinMax zmm, MinMax lmm,
		      float hit_density = 1.0/units::mm,
		      int dills_per_frame = 20);
	virtual ~DillDepositor();

	const PointValueVector& depositions(int frame_number) const;

    private:
	MinMax xmm, ymm, zmm, lmm;
	float hit_density, unit_charge;
	int dills_per_frame;

	// cache this long enough so the depositions() return can be used w/out copy
	// make it mutable so we can otherwise be const.
	mutable PointValueVector hits;
    };


}

#endif
