#ifndef WIRECELLNAV_EXAMPLEWIRES
#define WIRECELLNAV_EXAMPLEWIRES

#include "WireCellData/Units.h"

#include "WireCellIface/IWireGeometry.h"

namespace WireCell {

    /// Make on example set of wires
    WireCell::IWireGeometry* make_example_wires(float pitch  = 3*units::mm, 
						float angle  = 60.0*units::pi/180.0,
						float yextent= 1*units::meter, 
						float zextent= 1*units::meter);


    // write me:
    //WireCell::IWireGeometry* make_microboone_gds();
    //WireCell::IWireGeometry* make_dune35t_gds();
    //WireCell::IWireGeometry* make_dune100kt_gds();

}
#endif
