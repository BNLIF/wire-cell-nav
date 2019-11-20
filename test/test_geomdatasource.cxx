#include "WCPNav/GeomDataSource.h"
#include "WCPData/Units.h"

#include <iostream>

using namespace WCP;
using namespace std;

int main()
{
    GeomDataSource gds;

    int errors = 0;

    const int nwiresperplane = 10;
    int ident = 0;
    for (int iplane = 0; iplane < 3; ++iplane) {
	WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane);
	for (int ind = 0; ind < nwiresperplane; ++ind) {
	    int channel = iplane*nwiresperplane + ind;
	    GeomWire wire(ident, plane, ind, channel);
	    gds.add_wire(wire);
	    ident += 1;
	}
    }

    if (gds.get_wires().size() != ident) {
	cerr << "FAIL: got bogus number of wires" << endl;
	++errors;
    }

    ident = 0;
    for (int iplane = -1; iplane < 3; ++iplane) {
	WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane);
	int nwires = nwiresperplane;
	if (iplane < 0) {
	    plane = kUnknownWirePlaneType;
	    nwires = 3*nwiresperplane;
	}

	GeomWireSelection ws = gds.wires_in_plane(plane);
	sort_by_planeindex(ws);
	if (nwires != ws.size()) {
	    cerr << "FAIL: wrong number of wires in plane "<<plane<<", want:" << nwires << " got:" << ws.size() << endl;
	    ++errors;
	}
	if (plane == kUnknownWirePlaneType) {
	    continue;
	}

	cout << "Plane: " << plane << " has " << ws.size() << " wires" << endl;

        if (iplane<0) {
            continue;
        }

	for (int ind = 0; ind < nwiresperplane; ++ind) {
	    int channel = iplane*nwiresperplane + ind;

	    const GeomWire* wire = gds.by_ident(ident);
	    cout << "Wire: " << wire->ident() 
		 << " plane=" << wire->plane() 
		 << " index=" << wire->index() << endl;

	    if (!wire || wire->ident() != ident) { 
		cerr << "FAIL: wrong wire ident, want: " << ident << " got:" << wire->ident() << endl;
		++errors;
	    }

	    const GeomWireSelection& ws_chan = gds.by_channel(channel);
	    for (size_t iw = 0; iw < ws_chan.size(); ++iw) {
		const GeomWire* one = ws_chan[iw];
		if (!one || one->channel() != channel) { 
		    cerr << "FAIL: no wire #" <<iw<<" for channel " << channel << endl;
		    ++errors;
		}
	    }
	    
	    const GeomWire* other = ws[ind];
	    if (wire != other) {
		cerr << "FAIL: unexpected wire, ident: " 
		     << wire->ident() << " != " << other->ident()
		     << endl;
		++errors;
	    }
	    ident += 1;
	}

    }

    if (errors) {
	cerr << "FAILED: got " << errors << " errors" << endl;
	exit(errors);
    }

    return 0;
}
