#include "WireCellNav/FrameDataSource.h"

using namespace WireCell;

FrameDataSource::FrameDataSource()
    : frame()
    , bins_per_frame(0)
{
}

FrameDataSource::~FrameDataSource()
{
}

int FrameDataSource::next()
{
    return this->jump(frame.index+1);
}

WireCell::Frame& FrameDataSource::get()
{
    return frame;
}
const WireCell::Frame& FrameDataSource::get() const
{
    return frame;
}



