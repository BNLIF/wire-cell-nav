#include "WireCellNav/WireParams.h"
#include "WireCellNav/ParamWires.h"
#include "WireCellNav/WireSummary.h"
#include "WireCellNav/Drifter.h"
#include "WireCellNav/Digitizer.h"
#include "WireCellUtil/RangeFeed.h"

#include "WireCellUtil/Testing.h"
#include "WireCellUtil/TimeKeeper.h"

#include "MyDepo.h"
#include "MyDrifter.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TArrow.h"
#include "TLine.h"

using namespace WireCell;
using namespace std;


// fixme: this is an evil copy from deep inside the ParamWires
const int chid_plane_offset = 100000;

int chid2plane(int chid)
{
    return chid/chid_plane_offset; // integer division
}
int chid2chan(int chid)
{
    return chid - chid2plane(chid)*chid_plane_offset;
}


TH2F* make_frame_hist(IFrame::pointer frame, int plane)
{
    int ntraces = 0;
    int mintbin, maxtbin;
    int minchan, maxchan;
    for (auto trace : frame->range()) {
	int chid = trace->channel();
	int this_plane = chid2plane(chid);
	if (this_plane != plane) {
	    continue;
	}
	int chan = chid2chan(chid);
	if (!ntraces) {
	    mintbin = trace->tbin();
	    maxtbin = trace->tbin() + trace->charge().size();
	    maxchan = minchan = chan;
	    ++ntraces;
	    continue;
	}
	mintbin = min(mintbin, trace->tbin());
	maxtbin = max(maxtbin, trace->tbin() + int(trace->charge().size()));
	minchan = min(minchan, chan);
	maxchan = max(maxchan, chan);
	++ntraces;
    }

    //cout << "Frame " << frame->ident() << " tbin=[" << mintbin << "," << maxtbin << "],"
    //     << " chan=[" << minchan << "," << maxchan << "]" << endl;

    TH2F* hist = new TH2F(Form("frame_%d_%d", frame->ident(), plane),
			  Form("Frame %d, plane %d", frame->ident(), plane),
			  maxtbin-mintbin+1, mintbin, maxtbin+1,
			  maxchan-minchan+1, minchan, maxchan+1);
    hist->SetXTitle("Digitized time bin (tick)");
    hist->SetYTitle("Channel number in plane");
    
    for (auto trace : frame->range()) {
	int tbin = trace->tbin();
	int chid = trace->channel();
	int this_plane = chid2plane(chid);
	if (this_plane != plane) {
	    continue;
	}
	int chan = chid2chan(chid);
	for (auto q : trace->charge()) {
	    if (q <= 0.0) {
		++tbin;
		continue;
	    }
	    //cout << "Filling " << frame->ident() << " plane=" << plane
	    //<< " chan=" << chan << " tbin=" << tbin << " q=" << q << endl;
	    hist->Fill(tbin, chan, q);
	    ++tbin;
	}
    }
    return hist;
}

std::string dump(IFrame::pointer frame)
{
    stringstream ss;
    ss << "frame: " << frame->ident() << " @" << frame->time() << "s with "
       << boost::distance(frame->range()) << " traces\n";
    for (auto it : frame->range()) {
	ss << "\tchid=" << it->channel() << " tbin=" << it->tbin() << " bins=" << it->charge().size() << "\n";
    }
    return ss.str();
}

int main(int argc, char* argv[]) {

    bool interactive = argc > 1;


    TimeKeeper tk("test_digitizer");
    Track activity;
    make_track_4d(activity, 10, Ray(Point(10,0,0), Point(100,10,10)), 1);
    make_track_4d(activity, 120, Ray(Point( 1,0,0), Point( 2,-100,0)), 2);
    make_track_4d(activity, 99, Ray(Point(130,50,50), Point(11, -50,-30)), 3);


    BoundingBox bbox(Ray(Point(-1,-1,-1), Point(1,1,1)));
    for (auto depo : activity) {
	bbox(depo->pos());
    }
    Ray bb = bbox.bounds();
    cout << "Bounds: " << bb.first << " --> " << bb.second << endl;

    test_sort(activity);	// side effects needed by all tests
    cout << tk("sorted") << endl;
    test_feed(activity);
    cout << tk("collection") << endl;
    Track drifted;
    test_drifted(activity, drifted);
    cout << tk("transport") << endl;
    cout << tk.summary() << endl;


    WireParams wp;
    ParamWires pw;
    pw.generate(wp);
    IWireSummary::pointer ws(new WireSummary);
    AssertMsg(ws, "Null wire summary");

    IWireSink::pointer wsink = dynamic_pointer_cast<IWireSink>(ws);
    AssertMsg(wsink, "Null cast to wire sink");
    wsink->sink(pw.wires_range());

    RangeFeed<Track::iterator> feed(activity.begin(), activity.end());
    Drifter drifter(0, drift_velocity);
    drifter.connect(feed);

    Digitizer digitizer(1000, tick_time);
    digitizer.set_wiresummary(ws);
    digitizer.connect(drifter);

    TApplication* theApp = 0;
    if (interactive) {
	theApp = new TApplication ("test_digitizer",0,0);
    }
    TCanvas c("c","c",400,1200);
    c.Divide(1,3);

    IFrame::pointer p;
    int count = 0;
    while ((p=digitizer())) {
	AssertMsg(p,"got null frame");
	//cout << "Dumping frame: #" << count << " " << dump(p) << endl;
	++count;

	for (int ind=0; ind<3; ++ind) {
	    int iplane = ind+1;
	    c.cd(iplane);
	    TH2F* h2 = make_frame_hist(p, iplane);
	    h2->Draw("colz");

	}
    }


    if (theApp) {
	theApp->Run();
    }
    else {			// batch
	c.Print("test_digitizer.pdf");
    }






    return 0;
}
