#include "WireCellNav/ExampleGDS.h"

using namespace WireCell;

int main()
{
    GeomDataSource* gds = make_example_gds();
    if (!gds) {
	return 1;
    }
    if (!gds->get_wires().size()) {
	return 2;
    }
    return 0;
}
