#ifndef WIRECELLNAV_SLICEDATASOURCE_H
#define WIRECELLNAV_SLICEDATASOURCE_H

#include "WireCellNav/FrameDataSource.h"
#include "WireCellData/Slice.h"

namespace WireCellNav {

    /**
       SliceDataSource - deliver slices from a FrameDataSource
     */
    class SliceDataSource {
	WireCellNav::FrameDataSource& fds;
	int slicenum;
    public:
	SliceDataSource(WireCellNav::FrameDataSource& fds);
	virtual ~SliceDataSource();

	/// Return the number of slices in the current frame
	int size() const;

	/// Go to the given slice, return slice number or -1 on error
	int jump(int slice_number); 

	/// Go to the next slice, return its number or -1 on error
	int next();

	/// Get the current slice, returning its number
	int get(WireCellData::Slice& slice);

    };

}

#endif
