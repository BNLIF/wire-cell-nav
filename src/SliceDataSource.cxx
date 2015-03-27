#include "WireCellNav/SliceDataSource.h"

using namespace WireCell;

SliceDataSource::SliceDataSource(FrameDataSource& fds)
    : fds(fds)
    , slicenum(-1)
{
}
SliceDataSource::~SliceDataSource()
{
}

int SliceDataSource::size() const
{
    if (slicenum < 0) { return 0; }
}

int SliceDataSource::jump(int slice_number)
{
    
}

int SliceDataSource::next()
{
}

int SliceDataSource::get(Slice& slice)
{
}

