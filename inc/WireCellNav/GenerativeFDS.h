#ifndef WIRECELLNAV_GENERATIVEFDS
#define  WIRECELLNAV_GENERATIVEFDS

#include "WireCellNav/FrameDataSource.h"
#include "WireCellNav/GeomDataSource.h"
#include "WireCellNav/SimDataSource.h"
#include "WireCellNav/Depositor.h"
#include "WireCellData/Units.h"

namespace WireCell {

    /** A FrameDataSource which generates frames based on a depositor.
     */
    class GenerativeFDS : public FrameDataSource , public SimDataSource
    {
    public:
	/** Create a GenerativeFDS.
	 * 
	 * The dep is a Depositor.  See that class for details.
	 *
	 * The gds is a GeomDataSource that determines which wires may
	 * be "hit" by what is produced by the Depositor.
	 *
	 * The nframes set how many frames to assume is in the data
	 * stream or negative to run forever.
	 */	
	GenerativeFDS(const Depositor& dep, const GeomDataSource& gds,
		      int bins_per_frame = 1000, int nframes_total = -1,
		      float drift_speed = 1.6*units::millimeter/units::microsecond);

	virtual ~GenerativeFDS();

	virtual int size() const;

	/// Explicitly set the "frame" (event) to process.  Frame number returned or -1 on error.
	virtual int jump(int frame_number);

	virtual SimTruthSelection truth() const; 

    private:    
	const Depositor& dep;
	const GeomDataSource& gds;

	int bins_per_frame, max_frames;
	float drift_speed;

	mutable WireCell::SimTruthSet simtruth;
    };

}

#endif
