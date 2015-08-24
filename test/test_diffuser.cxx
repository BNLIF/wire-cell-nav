#include "WireCellUtil/BufferedHistogram2D.h"
#include "WireCellUtil/Testing.h"
#include "WireCellNav/Diffuser.h"

#include <iostream>

using namespace WireCell;
using namespace std;

void dump_hist(BufferedHistogram2D& hist)
{
    while (hist.size()) {
	double x = hist.xmin();
	std::vector<double> slice = hist.popx();
	cerr << "[" << hist.size() << "/" << slice.size()<<"] " << x;
	for (auto d : slice) {
	    cerr << " " << d;
	}
	cerr << endl;
    }
}

int main()
{
    BufferedHistogram2D hist;
    Diffuser diff(hist, 3);

    double xmin2 = diff.diffuse(15.0,10.0,1.0,1.0);
    cerr << "depo2 xmin = " << xmin2 << endl;

    double xmin1 = diff.diffuse(4.0,2.0,1.0,1.0);
    cerr << "depo1 xmin = " << xmin1 << endl;

    dump_hist(hist);

    return 0;

}
