#ifndef WIRECELLNAV_THRESHOLDFDS_H
#define WIRECELLNAV_THRESHOLDFDS_H

#include "WCPNav/FrameDataSource.h"
#include "WCPData/Frame.h"

namespace WCP {

/**
   ThresholdFDS - filter WCP::Frame data (Traces) provided via a FrameDataSource by
       using a single charge threshold.
 */
class ThresholdFDS : public WCP::FrameDataSource {
protected:

    WCP::FrameDataSource *_FDS;
    double _threshold;
    WCP::Frame frame;
    
public:
    ThresholdFDS(WCP::FrameDataSource &FDS, double threshold);
    ThresholdFDS(WCP::FrameDataSource &FDS);
    virtual ~ThresholdFDS();

    /// Return the number of frames this data source knows about.  Return -1 if unlimited.
    virtual int size() const = 0;

    /// Calls WCP::FrameDataSource::jump() and does threshold filtering
    virtual int jump(int frame_number);

    /// Advance to next frame, return frame number or -1 on error, -2 on end of frames
    virtual int next();

    /// Access currently loaded frame.
    virtual WCP::Frame& get();
    virtual const WCP::Frame& get() const;

};

}

#endif
