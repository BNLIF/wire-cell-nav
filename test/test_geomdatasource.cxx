#include "WireCellNav/GeomDataSource.h"

#include <iostream>

using namespace WireCellNav;
using namespace WireCellData;
using namespace std;

int main()
{
    GeomDataSource gds;

    int errors = 0;

    const int nwiresperplane = 10;
    int ident = 0;
    for (int iplane = 0; iplane < 3; ++iplane) {
	WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane+1);
	for (int ind = 0; ind < nwiresperplane; ++ind) {
	    ident += 1;
	    int channel = iplane*nwiresperplane + ind + 1;
	    Wire wire(ident, plane, ind, channel);
	    gds.add_wire(wire);
	}
    }

    if (gds.get_wires().size() != ident) {
	cerr << "got bogus number of wires" << endl;
	++errors;
    }

    ident = 0;
    for (int iplane = 0; iplane < 3; ++iplane) {
	WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane+1);

	WireSelection ws = gds.wires_in_plane(plane);
	WireCellData::sort_by_planeindex(ws);

	if (nwiresperplane != ws.size()) {
	    cerr << "Failed to get back the right number of wires" << endl;
	    ++errors;
	}

	cout << "Plane: " << plane << " has " << ws.size() << " wires" << endl;

	for (int ind = 0; ind < nwiresperplane; ++ind) {
	    ident += 1;
	    int channel = iplane*nwiresperplane + ind + 1;

	    const Wire* wire = gds.by_ident(ident);
	    cout << "Wire: " << wire->ident << " plane=" << wire->plane << " index=" << wire->index << endl;

	    if (!wire || wire->ident != ident) { 
		cerr << "Failed to get wire ident " << ident << endl;
		++errors;
	    }

	    wire = gds.by_channel(channel);
	    if (!wire || wire->channel != channel) { 
		cerr << "Failed to get wire channel " << channel << endl;
		++errors;
	    }
	    
	    const Wire* other = ws[ind];
	    if (wire != other) {
		cerr << "Got unexpected wire, ident: " 
		     << wire->ident << " != " << other->ident
		     << endl;
		++errors;
	    }
	}
    }

    if (errors) {
	cerr << "Got " << errors << " errors" << endl;
	exit(errors);
    }

    return 0;
}
