#include "WireCell/SliceDataSource.h"

WireCell::SliceDataSource::SliceDataSource(WireCell::FrameDataSource& fds)
    : fds(fds)
    , slicenum(-1)
{
}
WireCell::SliceDataSource::~SliceDataSource()
{
}

int WireCell::SliceDataSource::framenum() const
{
    return fds.framenum();
}

int WireCell::SliceDataSource::size() const
{
    if (slicenum < 0) { return 0; }
}

int WireCell::SliceDataSource::jump(int slice_number)
{
    
}

int WireCell::SliceDataSource::next()
{
}

int WireCell::SliceDataSource::get(WireCell::Slice& slice)
{
}

