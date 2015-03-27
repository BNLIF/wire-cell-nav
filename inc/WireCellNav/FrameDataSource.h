#ifndef WIRECELLNAV_FRAMEDATASOURCE_H
#define WIRECELLNAV_FRAMEDATASOURCE_H

#include "WireCellData/Trace.h"

namespace WireCell {

/**
   FrameDataSource - provide access to WireCell:Frame data

   This is an ABC, subclasses implement a real data source.
 */
class FrameDataSource {
public:
    FrameDataSource();
    virtual ~FrameDataSource();

    /// Return the number of frames this data source knows about.  Return -1 if unlimited.
    virtual int size() const = 0;

    /// Explicitly set the "frame" (event) to process.  Frame number returned or -1 on error.
    virtual int jump(int frame_number) = 0;

    /// Advance to next frame, return frame number or -1 on error, -2 on end of frames
    virtual int next() = 0;

    /// Fill full frame with current frame data, returning it's number
    virtual int get(WireCell::Frame& frame) const = 0;

};

}

#endif
