#include "WireCellNav/Digitizer.h"
#include "WireCellNav/ZSEndedTrace.h"
#include "WireCellNav/SimpleFrame.h"

#include <map>
#include <vector>

//#include <iostream>

using namespace WireCell;
using namespace std;

Digitizer::Digitizer(int maxticks, double tick, double start_time)
    : Signal<IDepo>()
    , m_maxticks(maxticks)
    , m_tick(tick)
    , m_time(start_time)
    , m_depo(0)
    , m_frame_count(0)
{
    
}

// a little helper class which produces ZSEndedTraces held in SimpleFrames
class TraceBuilder {

    std::vector<ZSEndedTrace*> m_traces;
    typedef std::map<int, int> TraceIndexMap; // channel->index into traces
    TraceIndexMap m_tim;

    IWireSummary::pointer m_ws;
    int m_nbins;

public:
    TraceBuilder(IWireSummary::pointer ws, int nbins)
	: m_ws(ws), m_nbins(nbins){}
    ~TraceBuilder() { }

    void add_depo(const Point& pos, int tbin, double charge) {
	for (int iplane=0; iplane<3; ++iplane) {
	    WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane);
	    IWire::pointer wire = m_ws->closest(pos, plane);
	    if (!wire) { continue; }

	    int chid = wire->channel();

	    TraceIndexMap::iterator it = m_tim.find(chid);
	    ZSEndedTrace* trace = 0;
	    if (it == m_tim.end()) {
		trace = new ZSEndedTrace(chid, m_nbins);
		m_tim[chid] = m_traces.size();
		m_traces.push_back(trace);
	    }
	    else {
		trace = m_traces[it->second];
	    }

	    (*trace)(tbin, charge);
	    
	}
    }
    IFrame* make_frame(int ident, double tmin) {
	std::vector<ITrace::pointer> exported(m_traces.begin(), m_traces.end());
	m_traces.clear();
	SequenceAdapter<ITrace> adapted(exported.begin(), exported.end());
	IFrame* ret = new SimpleFrame(ident, tmin, adapted);
	return ret;
    }
};


IFrame::pointer Digitizer::operator()()
{
    // prime
    if (!m_depo) { m_depo = fire(); }
    if (!m_depo) { return 0; }	// empty

    const double tmin = m_time;
    const double tmax = m_time + m_maxticks*m_tick;
    m_time += tmax;		// next start time

    TraceBuilder tm(m_wiresummary, m_maxticks);
    while (true) {
	if (!m_depo || m_depo->time() > tmax) {
	    IFrame* ret = tm.make_frame(m_frame_count, tmin);
	    ++m_frame_count;
	    //cerr << "Making frame" << endl;
	    IFrame::pointer result(ret);
	    return result;
	}
	
	if (m_depo->charge() > 0.0) {
	    int tbin = (m_depo->time() - tmin)/m_tick;
	    //cerr << "Add depo: t=" << m_depo->time() << " tmin=" << tmin
	    //     << " tick=" << m_tick << " tbin=" << tbin << endl;
	    tm.add_depo(m_depo->pos(), tbin, m_depo->charge());
	}

	m_depo = fire();	// set up for next
    }
}



