#ifndef WIRECELLNAV_REBINNINGFDS
#define  WIRECELLNAV_REBINNINGFDS

#include "WireCellNav/FrameDataSource.h"

namespace WireCell {

    class RebinningFDS : public FrameDataSource
    {
	FrameDataSource& fds;
	int nbins_to_group;
    public:
	RebinningFDS(FrameDataSource& fds, int nbins_to_group);
	virtual ~RebinningFDS();

	/// Explicitly set the "frame" (event) to process.  Frame number returned or -1 on error.
	virtual int jump(int frame_number);

    };
}

#endif
