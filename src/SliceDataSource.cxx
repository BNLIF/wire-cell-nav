#include "WireCellNav/SliceDataSource.h"

using namespace WireCell;

SliceDataSource::SliceDataSource(FrameDataSource& fds)
    : fds(fds)
    , frame_index(-1)
    , slice_index(-1)
    , slices_begin(-1)
    , slices_end(-1)
{
    update_slices_bounds();
}

SliceDataSource::~SliceDataSource()
{
}

void SliceDataSource::clear()
{
    slice_index = slices_begin = slices_end = -1;
    slice.clear();
}

// Update the bounds on the slice indices based on the current frame.
void SliceDataSource::update_slices_bounds() const
{
    const Frame& frame = fds.get();

    if (frame.index < 0) {	// no frames
	return;
    }

    if (frame.index == frame_index) { // no change, no-op
	return;
    }

    // frame change

    size_t ntraces = frame.traces.size();
    for (size_t ind=0; ind<ntraces; ++ind) {
	const Trace& trace = frame.traces[ind];

	if (!ind) {
	    slices_begin = trace.tbin;
	    slices_end   = trace.charge.size() + slices_begin;
	    continue;
	}
	int tbin = trace.tbin;
	int nbins = trace.charge.size();
	slices_begin = std::min(slices_begin, tbin);
	slices_end   = std::max(slices_end,   tbin + nbins);
    }
    return;
}

int SliceDataSource::size() const
{
    this->update_slices_bounds();
    if (slices_begin < 0 || slices_end < 0) {
	return 0;
    }
    return slices_end - slices_begin;
}

int SliceDataSource::jump(int index)
{
    if (index < 0) {		// underflow
	this->clear();
	return index;
    }

    this->update_slices_bounds();

    if (index >= slices_end) {	// overflow
	this->clear();
	return -1;
    }
    if (index == slice_index) {
	return index;		// already loaded, no-op
    }

    // new slice

    slice.clear();
    slice_index = index;	
    slice.tbin = slice_index + slices_begin; 
	
    const Frame& frame = fds.get();
    size_t ntraces = frame.traces.size();
    for (size_t ind=0; ind<ntraces; ++ind) {
	const Trace& trace = frame.traces[ind];

	int tbin = trace.tbin;
	int nbins = trace.charge.size();
	if (slice.tbin < tbin) {
	    continue;
	}
	if (slice.tbin >= tbin+nbins) {
	    continue;
	}

	// FIXME!!! this is wrong wrong wrong
	int wid = trace.chid;
	int q = trace.charge[slice.tbin];
	slice.charge.push_back(WireCharge(wid, q));
    }
    return index;
}

int SliceDataSource::SliceDataSource::next()
{
    this->jump(slice_index+1);
}

Slice& SliceDataSource::SliceDataSource::get()
{
    return slice;
}

const Slice& SliceDataSource::SliceDataSource::get() const
{
    return slice;
}

