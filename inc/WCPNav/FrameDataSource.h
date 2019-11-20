#ifndef WIRECELLNAV_FRAMEDATASOURCE_H
#define WIRECELLNAV_FRAMEDATASOURCE_H

#include "WCPData/Frame.h"

#ifdef WIRECELLSIGNAL_CONVOLUTEDRESPONSE_H
#include "WCPSignal/ConvolutedResponse.h"
#endif

namespace WCP {

/**
   FrameDataSource - provide base class access to WCP:Frame data.

   This class is mostly abstract.  Subclass implementations should 
 */
    class FrameDataSource {
    protected:

	WCP::Frame frame;
	int bins_per_frame;
	
    public:
	FrameDataSource();
	virtual ~FrameDataSource();
	
	void clear(){frame.clear();};

	/// Return the number of frames this data source knows about.  Return -1 if unlimited.
	virtual int size() const = 0;
	
	/// Explicitly set the "frame" (event) to process.  Frame number returned or -1 on error.
	virtual int jump(int frame_number) = 0;
	
	/// Advance to next frame, return frame number or -1 on error, -2 on end of frames
	virtual int next();
	
#ifdef WIRECELLSIGNAL_CONVOLUTEDRESPONSE_H
	virtual void SetResponseFunctions(WCPSignal::ConvolutedResponse *f) = 0;
#endif
	/// Access currently loaded frame.
	virtual WCP::Frame& get();
	virtual const WCP::Frame& get() const;

	int Get_Bins_Per_Frame(){return bins_per_frame;};
    };

}

#endif
