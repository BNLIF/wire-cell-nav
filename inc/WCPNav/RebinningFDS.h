#ifndef WIRECELLNAV_REBINNINGFDS
#define  WIRECELLNAV_REBINNINGFDS

#include "WCPNav/FrameDataSource.h"

namespace WCP {

    class RebinningFDS : public FrameDataSource
    {
    public:
	RebinningFDS(FrameDataSource& fds, int nbins_to_group);
	virtual ~RebinningFDS();

	/// Explicitly set the "frame" (event) to process.  Frame number returned or -1 on error.
	virtual int jump(int frame_number);

    private:
	FrameDataSource& fds;
	int nbins_to_group;
    };
}

#endif
