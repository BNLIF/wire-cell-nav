#include "WireCellNav/DillDepositor.h"

#include <random>

#include <iostream>
using namespace WireCell;
using namespace std;

DillDepositor::DillDepositor(MinMax xmm, MinMax ymm, MinMax zmm, MinMax lmm,
			     float hit_density, int dills_per_frame)
    : xmm(xmm), ymm(ymm), zmm(zmm), lmm(lmm)
    , hit_density(hit_density)
    , unit_charge(1.0)
    , dills_per_frame(dills_per_frame)
{
}
DillDepositor::~DillDepositor()
{
}

const PointValueVector& DillDepositor::depositions(int frame_number) const
{
    // fixme: we need to get a handle on rng usage across all code!
    std::default_random_engine gen(frame_number+1); // 0 and 1 seem to give the same randoms
    std::uniform_real_distribution<float> xrand(xmm.first, xmm.second);
    std::uniform_real_distribution<float> yrand(ymm.first, ymm.second);
    std::uniform_real_distribution<float> zrand(zmm.first, zmm.second);
    std::uniform_real_distribution<float> lrand(lmm.first, lmm.second);

    hits.clear();
    for (int ind=0; ind < dills_per_frame; ++ind) {
	float length = lrand(gen);

	Point p1(xrand(gen), yrand(gen), zrand(gen));
	hits.push_back(PointValue(p1, unit_charge));

	Point p2(xrand(gen), yrand(gen), zrand(gen));
	Point dir = Point(p2-p1).norm();

	for (float step = hit_density; step < length; step += hit_density) {
	    Point p = p1 + step * dir;
	    PointValue pv(p, unit_charge);
	    //cerr << "hit: " << ind << " q=" << pv.second << " p=" << p << endl;
	    hits.push_back(pv);
	}
    }
    return hits;
}
