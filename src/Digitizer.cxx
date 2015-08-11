#include "WireCellNav/Digitizer.h"

#include <map>

#include <iostream>

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


class DigitizedTrace : public ITrace {
    int m_chid;
    ChargeSequence m_charge;
public:
    DigitizedTrace(int chid, int nbins)
	: m_chid(chid)
	, m_charge(nbins,0)
    {
    }

    void operator()(int bin, float charge) {
	m_charge[bin] += charge;
    }

    virtual int channel() const { return m_chid; }

    virtual int tbin() const { return 0; }

    virtual const ChargeSequence& charge() const { return m_charge; };
};



class SimpleFrame : public IFrame {
    int m_ident;
    double m_time;
    std::vector<ITrace::pointer> m_traces;
public:
    SimpleFrame(int ident, double time, const std::vector<DigitizedTrace*>& traces)
	: m_ident(ident), m_time(time) {
	cerr << "SimpleFrame(" << ident << "," << time << "," << traces.size() << ")" << endl;

	for (auto dt : traces) {
	    m_traces.push_back(ITrace::pointer(dt));
	}
    }
    ~SimpleFrame() {
	cerr << "~SimpleFrame(" << m_ident << "," << m_time << "," << m_traces.size() << ")" << endl;
    }
    virtual int ident() const { return m_ident; }
    virtual double time() const { return m_time; }
    
    virtual iterator begin() { return adapt(m_traces.begin()); }
    virtual iterator end() { return adapt(m_traces.end()); }
};

class TraceBuilder {

    std::vector<DigitizedTrace*> traces;
    typedef std::map<int, int> TraceIndexMap; // channel->index into traces
    TraceIndexMap tim;

    IWireSummary::pointer ws;
    int nbins;
    int nframes;
public:
    TraceBuilder(IWireSummary::pointer ws, int nbins)
	: ws(ws), nbins(nbins), nframes(0) {}
    ~TraceBuilder() {
	for (auto t : traces) {
	    delete t;
	}
    }

    void add_depo(const Point& pos, int tbin, double charge) {
	for (int iplane=0; iplane<3; ++iplane) {
	    WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane);
	    IWire::pointer wire = ws->closest(pos, plane);
	    if (!wire) { continue; }

	    int chid = wire->channel();

	    TraceIndexMap::iterator it = tim.find(chid);
	    DigitizedTrace* trace = 0;
	    if (it == tim.end()) {
		trace = new DigitizedTrace(chid, nbins);
		tim[chid] = traces.size();
		traces.push_back(trace);
	    }
	    else {
		trace = traces[it->second];
	    }

	    (*trace)(tbin, charge);
	    
	}
    }
    IFrame* make_frame(int ident, double tmin) {
	IFrame* ret = new SimpleFrame(ident, tmin, traces);
	traces.clear();
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
	    cerr << "Making frame" << endl;
	    IFrame::pointer result(ret);
	    return result;
	}
	
	int tbin = (m_depo->time() - tmin)/m_tick;
	tm.add_depo(m_depo->pos(), tbin, m_depo->charge());

	m_depo = fire();	// set up for next
    }
}



