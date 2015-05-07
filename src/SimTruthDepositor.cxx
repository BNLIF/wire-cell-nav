#include "WireCellNav/SimTruthDepositor.h"

#include <iostream>		// debug
using namespace std;

using namespace WireCell;


SimTruthDepositor::SimTruthDepositor(SimDataSource& sds)
    : sds(sds)
    , hits()
    , last_frame(-1)
    , last_tbinmin(0)
    , last_tbinmax(0)
{
}
SimTruthDepositor::~SimTruthDepositor()
{
}


const PointValueVector& SimTruthDepositor::depositions(int frame_number) const
{
    static PointValueVector fail;
    if (sds.jump(frame_number) < 0) {
	return fail;
    }
    if (last_frame >= 0 && frame_number == last_frame) {
	return hits;		// cheap and easy caching
    }

    hits.clear();
    WireCell::SimTruthSelection sts = sds.truth();
    for (size_t itruth = 0; itruth < sts.size(); ++itruth) {
	const WireCell::SimTruth* st = sts[itruth];
	hits.push_back(PointValue(Point(st->tdc(), st->y(), st->z()), st->charge()));
    }
    cerr << "SimTruthDepositor depositing " << hits.size() << " hits" << endl;
    return hits;
}
