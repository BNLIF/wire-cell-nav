#include "WireCellNav/PlaneDuctor.h"
#include "WireCellNav/Drifter.h"
#include "WireCellUtil/Point.h"
#include "WireCellUtil/Testing.h"

#include <iostream>
#include <algorithm> 

using namespace WireCell;
using namespace std;

class SimpleDepo : public IDepo {
    double m_time;
    Point m_pos;
public:
    SimpleDepo(double t, const Point& pos) : m_time(t), m_pos(pos)  {}

    virtual const Point& pos() const { return m_pos; }
    virtual double time() const { return m_time; }
    virtual double charge() const { return 1.0; }
    virtual WireCell::IDepo::pointer prior() const { return 0; } // primordial
};


bool by_time_desc(const IDepo::pointer& lhs, const IDepo::pointer& rhs)
{
    if (lhs->time() == rhs->time()) {
	return lhs.get() > rhs.get();
    }
    return lhs->time() > rhs->time();
}


struct TrackDepos {

    const double stepsize;
    const double clight;
    shared_ptr<IDepoVector> depos;
    
    TrackDepos(double stepsize=0.1*units::millimeter,
	       double clight=1.0*units::meter/units::microsecond)
	: stepsize(stepsize), clight(clight), depos(new IDepoVector) {}

    void add_track(double time, const Ray& ray) {

	const Vector dir = ray_unit(ray);
	const double length = ray_length(ray);
	double step = 0;
	int count = 0;
	while (step < length) {
	    const double now = time + step/clight;
	    const Point here = ray.first + dir * step;
	    //cerr << "\tdepo: " << here << " @" << now << endl;
	    depos->push_back(IDepo::pointer(new SimpleDepo(now, here)));
	    step += stepsize;
	    ++count;
	}

	cerr << "add_track(t=" << time << ", pos=" << ray.first << " --> " << ray.second << ") " 
	     << count << " deposits, step=" << step << " length=" << length << endl;
	cerr << "\tlast: t=" << depos->back()->time() << " pos=" << depos->back()->pos() << endl;


	sort(depos->begin(), depos->end(), by_time_desc);
    }

    int left() { return depos->size(); };

    IDepo::pointer operator()() {
	if (!depos->size()) { return nullptr; }
	IDepo::pointer p = depos->back();
	depos->pop_back();
	//cerr << "Popping t=" << p->time() << ", pos=" << p->pos() << " with left: " << left() << endl;
	return p;
    }

    IDepoVector& depositions() { return *depos.get(); }
};


int main()
{
    // Describe a wire plane is at x=10 with 1 cm pitch
    const double plane_x = 1*units::cm;
    const double plane_pitch = 1*units::cm;
    const double plane_half_width = 10*units::cm;
    const double drift_velocity = 1.0*units::millimeter/units::microsecond;
    const double tick = 1.0*units::microsecond;

    Ray pitch(Point(plane_x,0,-plane_half_width),
	      Point(plane_x,0,-(plane_half_width - plane_pitch)));

    cerr << "Pitch: " << pitch.first << " --> " << pitch.second 
	 << " pitch=" << ray_length(pitch) 
	 << " total wires=" << (2*plane_half_width/plane_pitch)
	 << endl ;
    cerr << "Drift: to x=" << plane_x
	 << " tau=" << plane_x / drift_velocity 
	 << " velocity=" << drift_velocity << endl;

    TrackDepos td;

    const double cm = units::cm;
    td.add_track(1.0*units::microsecond, Ray(Point(plane_x+1*cm, 0, 0),       Point(plane_x+3*cm,0,0)));
    td.add_track(2.0*units::microsecond, Ray(Point(plane_x+2*cm,-1*cm,-1*cm), Point(plane_x+2*cm,cm,cm)));
    td.add_track(5.0*units::microsecond, Ray(Point(plane_x+20*cm,-10*cm,-10*cm), Point(plane_x+10*cm,10*cm,10*cm)));
    td.add_track(10.0*units::microsecond, Ray(Point(plane_x+10*cm,-10*cm,-10*cm), Point(plane_x+10*cm,10*cm,10*cm)));
    cerr << "TrackDepos loaded with " << td.left() << " depositions" << endl;


    double now = 0.0*units::microsecond;
    PlaneDuctor pd(pitch, tick, now, drift_velocity);

    for (auto depo : td.depositions()) {
	double tau = pd.proper_tau(depo->time(), depo->pos().x());
	double pdist = pd.pitch_dist(depo->pos());
	//cerr << "Depp: pos=" << depo->pos() << " time=" << depo->time() << " tau=" << tau << " pdist=" << pdist << endl;
	AssertMsg(tau>0, "Negative tau");
    }

    WireCell::Drifter drifter(plane_x, drift_velocity);
    drifter.connect(td);	// copies td


    pd.connect(drifter);	//copies drifter

    while (now < 10*units::microsecond) {
	double t = pd.clock();
	int n = pd.buffer_size();

	if (!n) {
	    cerr << "\n\nNo more data.\n";
	    break;
	}
	auto wq = pd.latch();
	if (!wq.size()) {
	    continue;
	}

	cerr << "Clock: " << t << " buffered: " << n << " ticks. "
	     << wq.size() << " wires: ";
	for (auto q : wq) {
	    cerr << q << " ";
	}
	cerr << " depos left=" << td.left()
	     << endl;
    }

}
