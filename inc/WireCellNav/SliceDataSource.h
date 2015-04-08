#ifndef WIRECELLNAV_SLICEDATASOURCE_H
#define WIRECELLNAV_SLICEDATASOURCE_H

#include "WireCellNav/FrameDataSource.h"
#include "WireCellNav/GeomDataSource.h"
#include "WireCellData/Slice.h"
#include "WireCellData/Frame.h"

namespace WireCell {

    /**
       SliceDataSource - deliver slices of frames from a FrameDataSource
     */
    class SliceDataSource {
    public:

	SliceDataSource(WireCell::FrameDataSource& fds);
	virtual ~SliceDataSource();

	/// Return the number of slices in the current frame.  
	int size() const;

	/// Go to the given slice, return slice number or -1 on error
	int jump(int slice_number); 

	/// Go to the next slice, return its number or -1 on error
	int next();

	/// Get the current slice
	WireCell::Slice&  get();
	const WireCell::Slice&  get() const;

    private:

	WireCell::FrameDataSource& _fds;

	WireCell::Slice _slice;	// cache the current slice
	int _frame_index;	// last frame we loaded
	int _slice_index;	// current slice, for caching
	mutable int _slices_begin; // tbin index of earliest bin of all traces
	mutable int _slices_end; // tbin index of one past the latest bin of all traces


	void update_slices_bounds() const;
	void clear();

    };

}

#endif
