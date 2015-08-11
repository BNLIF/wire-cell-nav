#ifndef WIRECELL_DRIFTER
#define WIRECELL_DRIFTER

#include "WireCellIface/IDepo.h"
#include "WireCellUtil/Units.h"
#include "WireCellUtil/Signal.h"

#include <boost/signals2.hpp>

namespace WireCell {

    class Drifter : public Signal<IDepo> {
    public:
	/// Create a drifter that will drift charge to a given
	/// location at a given drift velocity.
	Drifter(double location=0*units::meter,
		double drift_velocity = 1.6*units::mm/units::microsecond);
	// fixme: make configureable

	/// Produce the next drifted deposition.  Can be used as a
	/// DepoFeeder to connect to another DepoFeed.
	IDepo::pointer operator()();


    private:
	double m_location, m_drift_velocity;

	DepoTauSortedSet m_buffer;

	double proper_time(IDepo::pointer depo);

	// buffer management
	bool buffer();
	IDepo::pointer top();
	IDepo::pointer bot();
	IDepo::pointer pop();

    };


}

#endif
