#ifndef WIRECELLNAV_SLICEDATASOURCE_H
#define WIRECELLNAV_SLICEDATASOURCE_H

#include "WireCellNav/FrameDataSource.h"
#include "WireCellData/Slice.h"
#include "WireCellData/Frame.h"

namespace WireCell {

    /**
       SliceDataSource - deliver slices from a FrameDataSource
     */
    class SliceDataSource {
	WireCell::FrameDataSource& fds;
	WireCell::Slice slice;	// current slice
	int frame_index;	// last frame we loaded
	int slice_index;	// current slice, for caching
	mutable int slices_begin; // tbin index of earliest bin of all traces
	mutable int slices_end;	  // tbin index of one past the latest bin of all traces

	void update_slices_bounds() const;
	void clear();

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

    };

}

#endif
