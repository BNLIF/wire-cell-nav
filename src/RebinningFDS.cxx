
#include "WCPNav/RebinningFDS.h"

using namespace WCP;

RebinningFDS::RebinningFDS(FrameDataSource& fds, int nbins_to_group)
    : fds(fds)
    , nbins_to_group(nbins_to_group)
{
}
RebinningFDS::~RebinningFDS()
{
}

int RebinningFDS::jump(int frame_number)
{
    int got = fds.jump(frame_number);
    if (got < 0) {
	return got;
    }
    if (frame.index == got) {
	return got;		// same frame
    }

    frame.clear();		// win or lose, we start anew

    const WCP::Frame& other_frame = fds.get();

    for (std::size_t itrace = 0; itrace < other_frame.traces.size(); ++itrace) {
	const WCP::Trace& other_trace = other_frame.traces[itrace];
	WCP::Trace trace;
	trace.chid = other_trace.chid;

	// rebin
	std::size_t nother_bins = other_trace.charge.size();
	trace.tbin = other_trace.tbin / nbins_to_group;
	trace.charge.resize(nother_bins / nbins_to_group + 1, 0);
	for (std::size_t ibin=0; ibin<nother_bins; ++ibin) {
	    trace.charge[ibin/nbins_to_group] += other_trace.charge[ibin];
	}
	frame.traces.push_back(trace);
    }
    frame.index = other_frame.index;
    return frame.index;
}

