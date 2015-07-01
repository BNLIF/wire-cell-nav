#include "WireCellNav/GenerativeFDS.h"

using namespace WireCell;
using namespace std;

GenerativeFDS::GenerativeFDS(const Depositor& dep, const GeomDataSource& gds, 
			     int bins_per_frame1, int nframes_total,
			     float bin_drift_distance)
    : dep(dep)
    , gds(gds)
    , max_frames(nframes_total)
    , bin_drift_distance(bin_drift_distance)
{
  bins_per_frame = bins_per_frame1;
}


GenerativeFDS::~GenerativeFDS()
{
}

int GenerativeFDS::size() const { return max_frames; }

WireCell::SimTruthSelection GenerativeFDS::truth() const
{
    WireCell::SimTruthSelection ret;

    for (auto it = simtruth.begin(); it != simtruth.end(); ++it) {
	ret.push_back(& (*it));
    }
    return ret;
}


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
    simtruth.clear();

    const PointValueVector& hits = dep.depositions(frame_number);
    size_t nhits = hits.size();
    if (!nhits) {
	frame.index = frame_number; 
	return frame.index;
    }

    typedef map<int, int> TraceIndexMap; // channel->index into traces;
    TraceIndexMap tim;		// keep tabs on what channels we've seen already

    std::pair<double, double> xmm = gds.minmax(0);
    
    for (size_t ind=0; ind<nhits; ++ind) {
	const Point& pt = hits[ind].first;
	float charge = hits[ind].second;
	int tbin = int((pt.x-xmm.second)/bin_drift_distance);
	
	if (tbin >= bins_per_frame) {
	    //cerr << "GenerativeFDS: drop: tbin=" << tbin << " >= " << bins_per_frame << endl;
	    continue;
	}

	if (!gds.contained_yz(pt)) {
	    //cerr << "GenerativeFDS: drop: point not contained: " << pt << endl;
	    continue;
	}
	  
	WireCell::SimTruth st(pt.x, pt.y, pt.z, charge, tbin, simtruth.size());
	simtruth.insert(st);
	//cerr << "SimTruth: " << st.trackid() << " q=" << st.charge()
	//     << " tbin=" << tbin << " pos=" << st.pos() << endl;

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

