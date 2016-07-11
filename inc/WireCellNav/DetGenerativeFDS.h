#ifndef WIRECELLNAV_DETGENERATIVEFDS
#define  WIRECELLNAV_DETGENERATIVEFDS

#include "WireCellNav/FrameDataSource.h"
#include "WireCellNav/DetectorGDS.h"
#include "WireCellNav/WrappedGDS.h"
#include "WireCellNav/SimDataSource.h"
#include "WireCellNav/Depositor.h"
#include "WireCellData/Units.h"
#include "WireCellData/Vector.h"

namespace WireCell {

    /** A FrameDataSource which generates frames of traces based on
     * hits from a Depositor object.
     *
     * This FDS models an infinite volume with wire planes embedded at
     * the locations given by the GeomDataSource.  Hits from provided
     * by the Depositor object are assumed to drift in the negative X
     * direction towards the wire planes at a constant
     * bin_drift_distance per digitized time bin.  A Frame of the
     * given bins_per_frame is digitized and hits are collected onto
     * wires.  Any hits outside the frame are ignored.
     */
    class DetGenerativeFDS : public FrameDataSource , public SimDataSource
    {
    public:
	/** Create a GenerativeFDS.
	 * 
	 * The dep is a Depositor.  See that class for details.
	 *
	 * The gds is a GeomDataSource that determines which wires may
	 * be "hit" by what is produced by the Depositor.
	 *
	 * The bins_per_frame determines how deep in the drift
	 * distance a single frame is.
	 *
	 * The nframes set how many frames to assume is in the data
	 * stream or negative to run forever.
	 *
	 * The bin_drift_distance is how far an electron will drift in
	 * the time collected in one time bin.  Ie, digitization
	 * period * drift speed.
	 */	
        DetGenerativeFDS(const Depositor& dep, const DetectorGDS& gds,
			 int bins_per_frame1 = 1000, int nframes_total = -1,
			 float bin_drift_distance = 0.5*1.6*units::millimeter, float unit_dis=1.6);

	virtual ~DetGenerativeFDS();

	virtual int size() const;

	/// Explicitly set the "frame" (event) to process.  Frame number returned or -1 on error.
	virtual int jump(int frame_number); 

	virtual SimTruthSelection truth() const; 

	void clear(){frame.clear();simtruth.clear();};
	double *sideband_charge[2];
	double tot_true_charge() {return tot_charge;}
	double tot_collected_charge() {return collected_charge;}	
	
    private:    
	const Depositor& dep;
	const DetectorGDS& det_gds;
	
	int  max_frames;
	float bin_drift_distance;
	float unit_dis;
	double tot_charge;
	double collected_charge;

	mutable WireCell::SimTruthSet simtruth;
    };

}

#endif
