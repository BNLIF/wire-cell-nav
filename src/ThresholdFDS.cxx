#include "WCPNav/ThresholdFDS.h"

using namespace WCP;

ThresholdFDS::ThresholdFDS(WCP::FrameDataSource &FDS, double threshold)
  : _FDS(&FDS)
  , _threshold(threshold)
  , frame()
{
}

ThresholdFDS::ThresholdFDS(WCP::FrameDataSource &FDS)
  : _FDS(&FDS)
  , frame()
{
    _threshold = 0.0;
}

ThresholdFDS::~ThresholdFDS()
{
}

int ThresholdFDS::next()
{
    return this->jump(frame.index+1);
}

WCP::Frame& ThresholdFDS::get()
{
    return frame;
}
const WCP::Frame& ThresholdFDS::get() const
{
    return frame;
}

int WCP::ThresholdFDS::jump(int frame_number)
{
    // jump to frame of interest
    frame.index = _FDS->jump(frame_number);

    // filter out trace charges below threshold, leading to splitting of traces
    int ntraces = frame.traces.size();
    int ntbins;
    int tbin_start;
    int tbin_end;
    int tbin;
    double charge;
    bool traceflag;
    WCP::Trace trace;
    WCP::Trace newtrace;
    for (int ind = 0; ind < ntraces; ++ind) {
        trace = frame.traces.at(ind);
        ntbins = trace.charge.size();

        newtrace.chid = trace.chid;
        newtrace.tbin = trace.tbin;

        traceflag = true;
        tbin_start = trace.tbin;
        tbin_end = tbin_start + ntbins - 1;

        tbin = tbin_start;
        while (tbin <= tbin_end) {
   	    charge = trace.charge.at(tbin-tbin_start);

            if (traceflag == true) {
	        if (charge >= _threshold) {    // continue filling a new trace
		    newtrace.charge.push_back(charge);

                    if (tbin == tbin_end) {    // end of original trace, so finish
                      frame.traces.push_back(newtrace);
		    }                    
	        }
	        else {                         // finish a new trace
		    traceflag = false;
        	    frame.traces.push_back(newtrace);
                    newtrace.charge.clear();
	        }
	    }
	    else {
                if (charge >= _threshold) {    // start a new trace
		    traceflag = true;
                    newtrace.tbin = tbin;
		    newtrace.charge.push_back(charge);

                    if (tbin == tbin_end) {    // end of original trace, so finish
                      frame.traces.push_back(newtrace);
		    }                    
	        }
	    }

            tbin++;
	}
    }

    // return frame index
    return frame.index;
}
