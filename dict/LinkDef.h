#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;
#pragma link C++ nestedfunctions;

#pragma link C++ class WireCell::GeomDataSource-!;
#pragma link C++ class WireCell::ParamGDS-!;
#pragma link C++ class WireCell::GenerativeFDS-!;
#pragma link C++ class WireCell::PepperDepositor-!;
#pragma link C++ class WireCell::DillDepositor-!;
#pragma link C++ class WireCell::FrameDataSource-!;
#pragma link C++ class WireCell::ThresholdFDS-!;
#pragma link C++ class WireCell::SliceDataSource-!;

#pragma link C++ function WireCell::make_example_gds;

#endif
