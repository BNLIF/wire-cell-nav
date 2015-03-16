#include "WireCellNav/SliceDataSource.h"

WireCellNav::SliceDataSource::SliceDataSource(WireCellNav::FrameDataSource& fds)
    : fds(fds)
    , slicenum(-1)
{
}
WireCellNav::SliceDataSource::~SliceDataSource()
{
}

int WireCellNav::SliceDataSource::size() const
{
    if (slicenum < 0) { return 0; }
}

int WireCellNav::SliceDataSource::jump(int slice_number)
{
    
}

int WireCellNav::SliceDataSource::next()
{
}

int WireCellNav::SliceDataSource::get(WireCellData::Slice& slice)
{
}

