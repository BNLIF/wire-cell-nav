#ifndef WIRECELL_ZSTRACE
#define  WIRECELL_ZSTRACE

namespace WireCell {

class ZSTrace : public ITrace {
    int m_chid, m_nbins;
    std::map<int,float> m_chqmap; // do simple zero suppression but only on the ends of the trace.
    mutable ChargeSequence m_charge;
public:
    ZSTrace(int chid, int nbins=0)
	: m_chid(chid)
	, m_nbins(nbins)
    {
    }

    void operator()(int bin, float charge) {
	m_chqmap[bin] += charge;
	m_charge.clear();	// invalidate;
    }

    virtual int channel() const { return m_chid; }

    virtual int tbin() const { return m_chqmap.begin()->first; }

    virtual const ChargeSequence& charge() const {
	if (!m_charge.size()) {
	    int first_bin = m_chqmap.begin()->first;
	    int last_bin = m_chqmap.rbegin()->first;
	    m_charge.resize(last_bin-first_bin+1, 0);
	    for (auto mit : m_chqmap) {
		m_charge[mit.first - first_bin] = mit.second;
	    }
	}
	return m_charge; 
    };
};

    

}

#endif
