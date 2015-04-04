#include "WireCellNav/SliceDataSource.h"

using namespace WireCell;

SliceDataSource::SliceDataSource(FrameDataSource& fds, const GeomDataSource& gds)
    : _fds(fds)
    , _gds(gds)
    , _frame_index(-1)
    , _slice_index(-1)
    , _slices_begin(-1)
    , _slices_end(-1)
{
    update_slices_bounds();
}

SliceDataSource::~SliceDataSource()
{
}

void SliceDataSource::clear()
{
    _slice_index = _slices_begin = _slices_end = -1;
    _slice.clear();
}

// Update the bounds on the slice indices based on the current frame.
void SliceDataSource::update_slices_bounds() const
{
    const Frame& frame = _fds.get();

    if (frame.index < 0) {	// no frames
	return;
    }

    if (frame.index == _frame_index) { // no change, no-op
	return;
    }

    // frame change

    size_t ntraces = frame.traces.size();
    for (size_t ind=0; ind<ntraces; ++ind) {
	const Trace& trace = frame.traces[ind];

	if (!ind) {		// first time
	    _slices_begin = trace.tbin;
	    _slices_end   = trace.charge.size() + _slices_begin;
	    continue;
	}
	int tbin = trace.tbin;
	int nbins = trace.charge.size();
	_slices_begin = std::min(_slices_begin, tbin);
	_slices_end   = std::max(_slices_end,   tbin + nbins);
    }
    return;
}

int SliceDataSource::size() const
{
    this->update_slices_bounds();
    if (_slices_begin < 0 || _slices_end < 0) {
	return 0;
    }
    return _slices_end - _slices_begin;
}

int SliceDataSource::jump(int index)
{
    if (index < 0) {		// underflow
	this->clear();
	return index;
    }

    this->update_slices_bounds();

    if (index >= _slices_end) {	// overflow
	this->clear();
	return -1;
    }
    if (index == _slice_index) {
	return index;		// already loaded, no-op
    }

    // new slice

    _slice.clear();
    _slice_index = index;	
    int slice_tbin = _slice_index + _slices_begin; 
    Wire::Group slice_group;
	
    const Frame& frame = _fds.get();
    size_t ntraces = frame.traces.size();
    for (size_t ind=0; ind<ntraces; ++ind) {
	const Trace& trace = frame.traces[ind];
	int tbin = trace.tbin;
	int nbins = trace.charge.size();

	if (slice_tbin < tbin) {
	    continue;
	}
	if (slice_tbin >= tbin+nbins) {
	    continue;
	}

	// Save association of a wire (segment) ID and charge.  Note,
	// the inherent degeneracy in detectors with wrapped wires is
	// preserved here when the WireSelection is larger than 1 wire.
	int q = trace.charge[slice_tbin];
	const GeomWireSelection& ws = _gds.by_channel(trace.chid);
	size_t ws_size = ws.size();
	for (size_t iwid=0; iwid < ws_size; ++iwid) {
	    slice_group.push_back(Wire::Charge(ws[iwid], q));
	}
    }
    _slice.reset(slice_tbin, slice_group);
    return index;
}

int SliceDataSource::SliceDataSource::next()
{
    this->jump(_slice_index+1);
}

Slice& SliceDataSource::SliceDataSource::get()
{
    return _slice;
}

const Slice& SliceDataSource::SliceDataSource::get() const
{
    return _slice;
}

