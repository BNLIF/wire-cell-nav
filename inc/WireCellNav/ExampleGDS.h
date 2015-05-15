#ifndef WIRECELLNAV_EXAMPLEGDS
#define WIRECELLNAV_EXAMPLEGDS

#include "WireCellNav/GeomDataSource.h"
#include "WireCellData/Units.h"


namespace WireCell {

    /// Make a small GDS
WireCell::GeomDataSource* make_example_gds(float pitch  = 3*units::mm, 
					   float angle  = 60.0*units::pi/180.0,
					   float yextent= 1*units::meter, 
					   float zextent= 1*units::meter);


//GeomDataSource* make_microboone_gds();
//GeomDataSource* make_dune35t_gds();
//GeomDataSource* make_dune100kt_gds();

}
#endif
