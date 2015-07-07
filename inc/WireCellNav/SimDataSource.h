#ifndef WIRECELLNAV_SIMDATASOURCE
#define WIRECELLNAV_SIMDATASOURCE

#include "WireCellData/SimTruth.h"

namespace WireCell {

    /** SimDataSource - a Mixin providing access to simulated truth information.
     *
     * You can inherit from this class, usually with a subclass that
     * is also a FrameDataSource, in order to grant access to any
     * simulation truth information in the form of a SimTruth object.
     *
     * Determining which sim truth is current is not defined here
     * (probably you will use FrameDataSource::jump().
     */
    class SimDataSource {
    public:
	virtual ~SimDataSource();

	/// Access to the sim truth objects.  The SimTruthSelection will be empty on error
	virtual SimTruthSelection truth() const = 0; 
	
	/// Explicitly set the "frame" (event) to process.  Frame number returned or -1 on error.
	virtual int jump(int frame_number) = 0;

    };

}

#endif
