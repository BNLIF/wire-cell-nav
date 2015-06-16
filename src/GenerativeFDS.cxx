#include "WireCellNav/GenerativeFDS.h"

using namespace WireCell;
using namespace std;

GenerativeFDS::GenerativeFDS(const Depositor& dep, const GeomDataSource& gds, 
			     int bins_per_frame, int nframes_total)
    : dep(dep)
    , gds(gds)
    , bins_per_frame(bins_per_frame)
    , max_frames(nframes_total)
{
}


GenerativeFDS::~GenerativeFDS()
{
}

int GenerativeFDS::size() const { return max_frames; }

int GenerativeFDS::jump(int frame_number)
{
    if (frame_number < 0) {
	return -1;
    }
    if (max_frames >=0 && frame_number >= max_frames) {
	return -1;
    }

    if (frame.index == frame_number) {
	return frame_number;
    }

    frame.clear();

    const PointValueVector& hits = dep.depositions(frame_number);

    size_t nhits = hits.size();

    typedef map<int, int> TraceIndexMap; // channel->index into traces;
    TraceIndexMap tim;		// keep tabs on what channels we've seen already

    for (size_t ind=0; ind<nhits; ++ind) {
	const Point& pt = hits[ind].first;
	float charge = hits[ind].second;
	int tbin = pt.x;
	
	if (tbin >= bins_per_frame) {
	    continue;
	}

	if (!gds.contained_yz(pt)) {
	    continue;
	}
	  
	for (int iplane=0; iplane < 3; ++iplane) {
	    WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane); // annoying
	    const GeomWire* wire = gds.closest(pt, plane);
	    int chid = wire->channel();
	  
	    TraceIndexMap::iterator it = tim.find(chid);
	  
	    int trace_index = frame.traces.size(); // if new
	    if (it == tim.end()) {
		Trace t;
		t.chid = chid;
		t.tbin = 0;
		t.charge.resize(bins_per_frame, 0.0);
		tim[chid] = frame.traces.size();
		frame.traces.push_back(t);
	    }
	    else {		// already seen
		trace_index = it->second;
	    }
	    Trace& trace = frame.traces[trace_index];
	    
	    // finally
	    trace.charge[tbin] += charge;
	}	
    }
    
    frame.index = frame_number; 
    return frame.index;
}

