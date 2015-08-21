#ifndef WIRECELL_DIFUSER
#define WIRECELL_DIFUSER

#include "WireCellUtil/BufferedHistogram2D.h"
#include "WireCellUtil/Point.h"
#include "WireCellUtil/Units.h"
#include "WireCellIface/IDepo.h"

namespace WireCell {


    /** Model diffusion of drifting charge.
     *
     * Every point deposition given is diffused along the directions
     * transverse and longitudinal to the drift direction (assumed to
     * be -X).  The amount of diffusion depends on the drift time from
     * the initial deposition point to the given reference plane
     * (determined by the pitch ray).  The diffusion is characterized
     * by a longitudinal and transverse constant D.  The Gaussian
     * width in time sigma = sqrt(2*D*t)/v with v the drift velocity
     * and t the drift time.
     */

    class Diffuser { 		// fixme: make interface?
	BufferedHistogram2D& m_hist;
	const Ray m_pitch;
	const Vector m_pitch_unit;
	const double m_drift_velocity;
	const double m_pitch_dist;
	const double m_DL;
	const double m_DT;
    public:
	Diffuser(BufferedHistogram2D& hist, Ray pitch,
		 double drift_velocity = 1.6*units::meter/units::millisecond,
		 double DL = 5.3*units::centimeter2/units::second,
		 double DT = 12.8*units::centimeter2/units::second);
	~Diffuser();
	
	double diffuse(IDepo::pointer depo);

    };

}

#endif

