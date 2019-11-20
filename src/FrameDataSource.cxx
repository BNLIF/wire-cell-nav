#include "WCPNav/FrameDataSource.h"

using namespace WCP;

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

WCP::Frame& FrameDataSource::get()
{
    return frame;
}
const WCP::Frame& FrameDataSource::get() const
{
    return frame;
}



