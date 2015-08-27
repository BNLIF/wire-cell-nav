//#include "WireCellNav/Digitizer.h"
#include "WireCellNav/PlaneDuctor.h"
#include "WireCellNav/Drifter.h"
#include "WireCellNav/Digitizer.h"
#include "WireCellNav/TrackDepos.h"
#include "WireCellNav/WireParams.h"
#include "WireCellNav/ParamWires.h"

#include "WireCellIface/WirePlaneId.h"

#include "WireCellUtil/Point.h"
#include "WireCellUtil/Faninout.h"
#include "WireCellUtil/Testing.h"

#include <iostream>

using namespace WireCell;
using namespace std;

TrackDepos make_tracks() {
    TrackDepos td;

    const double cm = units::cm;
    td.add_track( 1.0*units::microsecond, Ray(Point(100*cm, 0, 0),         Point(103*cm,0,0)));
    td.add_track( 2.0*units::microsecond, Ray(Point(102*cm,-1*cm,-1*cm),   Point(102*cm,cm,cm)));
    td.add_track( 5.0*units::microsecond, Ray(Point(120*cm,-10*cm,-10*cm), Point(110*cm,10*cm,10*cm)));
    td.add_track(10.0*units::microsecond, Ray(Point(110*cm,-10*cm,-10*cm), Point(110*cm,10*cm,10*cm)));
    return td;
}


int main()
{
    const double tick = 2.0*units::microsecond;
    double now = 0.0*units::microsecond;

    IWireParameters::pointer iwp(new WireParams);
    ParamWires pw;
    pw.generate(iwp);

    TrackDepos td = make_tracks();
    Fanout<IDepo::pointer> depofan;
    depofan.address(0);
    depofan.address(1);
    depofan.address(2);

    Addresser<IDepo::pointer> depoU(0), depoV(1), depoW(2);

    PlaneDuctor pdU(WirePlaneId(kUlayer), iwp->pitchU(), tick, now);
    PlaneDuctor pdV(WirePlaneId(kVlayer), iwp->pitchV(), tick, now);
    PlaneDuctor pdW(WirePlaneId(kWlayer), iwp->pitchW(), tick, now);

    WireCell::Drifter driftU(iwp->pitchU().first.x());
    WireCell::Drifter driftV(iwp->pitchV().first.x());
    WireCell::Drifter driftW(iwp->pitchW().first.x());

    Digitizer digitizer;
    digitizer.sink(pw.wires_range());

    // Now connect up the nodes

    // fan out the depositions
    depofan.connect(td);

    // address the fan-out
    depoU.connect(boost::ref(depofan));
    depoV.connect(boost::ref(depofan));
    depoW.connect(boost::ref(depofan));

    // drift each to the plane
    driftU.connect(boost::ref(depoU));
    driftV.connect(boost::ref(depoV));
    driftW.connect(boost::ref(depoW));

    // diffuse and digitize
    pdU.connect(boost::ref(driftU));
    pdV.connect(boost::ref(driftV));
    pdW.connect(boost::ref(driftW));

    // aggregate and digitize
    digitizer.connect(boost::ref(pdU));
    digitizer.connect(boost::ref(pdV));
    digitizer.connect(boost::ref(pdW));

    // process
    while (true) {
	auto slice = digitizer();
	if (!slice) { break; }
	IChannelSlice::ChannelCharge cc = slice->charge();
	cout << "Digitized t=" << slice->time() << " #ch=" << cc.size() << endl;
    }

    return 0;
}
