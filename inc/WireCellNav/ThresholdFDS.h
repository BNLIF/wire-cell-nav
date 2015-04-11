#ifndef WIRECELLNAV_THRESHOLDFDS_H
#define WIRECELLNAV_THRESHOLDFDS_H

#include "WireCellNav/FrameDataSource.h"
#include "WireCellData/Frame.h"

namespace WireCell {

/**
   ThresholdFDS - filter WireCell::Frame data (Traces) provided via a FrameDataSource by
       using a single charge threshold.
 */
class ThresholdFDS : public WireCell::FrameDataSource {
protected:

    WireCell::FrameDataSource *_FDS;
    double _threshold;
    WireCell::Frame frame;
    
public:
    ThresholdFDS(WireCell::FrameDataSource &FDS, double threshold);
    ThresholdFDS(WireCell::FrameDataSource &FDS);
    virtual ~ThresholdFDS();

    /// Return the number of frames this data source knows about.  Return -1 if unlimited.
    virtual int size() const = 0;

    /// Calls WireCell::FrameDataSource::jump() and does threshold filtering
    virtual int jump(int frame_number);

    /// Advance to next frame, return frame number or -1 on error, -2 on end of frames
    virtual int next();

    /// Access currently loaded frame.
    virtual WireCell::Frame& get();
    virtual const WireCell::Frame& get() const;

};

}

#endif
