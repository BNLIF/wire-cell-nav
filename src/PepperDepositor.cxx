#include "WireCellNav/PepperDepositor.h"

#include <random>

#include <iostream>
using namespace WireCell;
using namespace std;

PepperDepositor::PepperDepositor(MinMax ymm, MinMax zmm, MinMax qmm,
				 int hits_per_frame, int tbins_per_frame)
    : ymm(ymm), zmm(zmm), qmm(qmm)
    , hits_per_frame(hits_per_frame)
    , tbins_per_frame(tbins_per_frame)
{
}
PepperDepositor::~PepperDepositor()
{
}

const PointValueVector& PepperDepositor::depositions(int frame_number) const
{
    // fixme: we need to get a handle on rng usage across all code!
    std::default_random_engine gen(frame_number+1); // 0 and 1 seem to give the same randoms
    std::uniform_real_distribution<float> trand(0, tbins_per_frame);
    std::uniform_real_distribution<float> yrand(ymm.first, ymm.second);
    std::uniform_real_distribution<float> zrand(zmm.first, zmm.second);
    std::uniform_real_distribution<float> qrand(qmm.first, qmm.second);

    hits.clear();
    for (int ind=0; ind < hits_per_frame; ++ind) {
	Point p(trand(gen), yrand(gen), zrand(gen));
	PointValue pv(p, qrand(gen));
	//cerr << "hit: " << ind << " q=" << pv.second << " p=" << p << endl;
	hits.push_back(pv);
    }
    return hits;
}
