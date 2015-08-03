#ifndef WIRECELL_DRIFTER
#define WIRECELL_DRIFTER

#include "WireCellIface/IDepo.h"
#include "WireCellUtil/Units.h"

#include <boost/signals2.hpp>

namespace WireCell {

    class Drifter {
	typedef boost::signals2::signal<IDepo::const_ptr ()> DepoFeed;
    public:

	/// A slot which feeds the drifter depositions.  Each time it
	/// is called it should return the next available deposition
	/// in strict time order.
	typedef DepoFeed::slot_type DepoFeeder;
	
	/// Create a drifter that will drift charge to a given
	/// location at a given drift velocity.
	Drifter(double location=0*units::meter,
		double drift_velocity = 1.6*units::mm/units::microsecond);
	// fixme: make configureable


	/// Connect a deposition feeder to me.
	void connect(const DepoFeeder& feed);
	// fixme: add to an Interface


	/// Produce the next drifted deposition.  Can be used as a
	/// DepoFeeder to connect to another DepoFeed.
	IDepo::const_ptr operator()();


    private:
	double m_location, m_drift_velocity;

	DepoTauSortedSet m_buffer;
	DepoFeed m_feed;

	double proper_time(IDepo::const_ptr depo);

	// buffer management
	bool buffer();
	IDepo::const_ptr top();
	IDepo::const_ptr bot();
	IDepo::const_ptr pop();

    };


}

#endif
