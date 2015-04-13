#ifndef WIRECELLNAV_GENERATIVEFDS
#define  WIRECELLNAV_GENERATIVEFDS

#include "WireCellNav/FrameDataSource.h"
#include "WireCellNav/GeomDataSource.h"
#include "WireCellNav/Depositor.h"

namespace WireCell {

    /** A FrameDataSource which generates frames based on a depositor.
     */
    class GenerativeFDS : public FrameDataSource 
    {
    public:
	/** Create a GenerativeFDS.
	 * 
	 * The dep is a Depositor.  See that class for details.
	 *
	 * The gds is a GeomDataSource that determines which wires may
	 * be "hit" by what is produced by the Depositor.
	 * 
	 * The bins_per_frame gives the number of times bins that each
	 * bin should contain.
	 *
	 * The nframes set how many frames to assume is in the data
	 * stream or negative to run forever.
	 */	
	GenerativeFDS(const Depositor& dep, const GeomDataSource& gds, 
		      int bins_per_frame = 1000, int nframes_total = -1);
	virtual ~GenerativeFDS();

	/// Explicitly set the "frame" (event) to process.  Frame number returned or -1 on error.
	virtual int jump(int frame_number);

    private:    
	const Depositor& dep;
	const GeomDataSource& gds;

	int bins_per_frame, max_frames;
    };

}

#endif
