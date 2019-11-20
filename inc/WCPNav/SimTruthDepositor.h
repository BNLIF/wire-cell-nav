#ifndef WIRECELLNAV_SIMTRUTHDEPOSITOR
#define WIRECELLNAV_SIMTRUTHDEPOSITOR

#include "WCPNav/Depositor.h"
#include "WCPNav/SimDataSource.h"

namespace WCP {

    /** SimTruthDepositor - deposit hits using SimTruth. 
     */
    class SimTruthDepositor : virtual public Depositor {
    public:
	SimTruthDepositor(SimDataSource& sds);
	virtual ~SimTruthDepositor();

	/// Depositor interface
	virtual const PointValueVector& depositions(int frame_number) const;


    private:
	SimDataSource& sds;
	mutable PointValueVector hits;
	mutable int last_frame, last_tbinmin, last_tbinmax;
    };
}

#endif
