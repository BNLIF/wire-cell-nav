#include "WireCellNav/SimpleFrame.h"

using namespace WireCell;


SimpleFrame::SimpleFrame(int ident, double time, ITraceSequence& traces)
    : m_ident(ident), m_time(time)
{
    for (auto trace : traces) {
	m_traces.push_back(trace);
    }
}
SimpleFrame::~SimpleFrame() {

}
int SimpleFrame::ident() const { return m_ident; }
double SimpleFrame::time() const { return m_time; }
    
SimpleFrame::iterator SimpleFrame::begin() { return adapt(m_traces.begin()); }
SimpleFrame::iterator SimpleFrame::end() { return adapt(m_traces.end()); }


