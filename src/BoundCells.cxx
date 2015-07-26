#include "WireCellNav/BoundCells.h"
#include "WireCellUtil/NamedFactory.h"
#include "WireCellUtil/Point.h"
#include "WireCellUtil/Units.h"

#include <boost/range.hpp>
#include <iterator>

#include <iostream>		// debug
using namespace std;

using namespace WireCell;

WIRECELL_NAMEDFACTORY(BoundCells);
WIRECELL_NAMEDFACTORY_ASSOCIATE(BoundCells, ICellGenerator);
WIRECELL_NAMEDFACTORY_ASSOCIATE(BoundCells, ICellProvider);


// This is the actual implementation of the cell we provide.  It is
// buried in this implementation for a reason.  It's not a publicly
// accessible class.
namespace WireCell {
    class BoundCell : public ICell {
    
	int m_ident;
	PointVector m_corners;	// this is likely a source of a lot of memory usage
	const std::vector<const IWire*> m_wires;

    public:
	BoundCell(int id, const PointVector& pcell, const std::vector<const IWire*>& wires)
	    : m_ident(id), m_corners(pcell), m_wires(wires) {}
	virtual ~BoundCell() {}	// do not further inherit.

	virtual int ident() const { return m_ident; }

	virtual double area() const { return 0.0; /*fixme*/}

	virtual WireCell::Point center() const {
	    /// fixme

	    WireCell::Point center;
	    size_t npos = m_corners.size();
	    for (int ind=0; ind<npos; ++ind) {
		center = center + m_corners[ind];
	    }
	    double norm = 1.0/npos;
	    return  norm * center;
	}

	virtual WireCell::PointVector corners() const {
	    return m_corners;
	}

//    virtual WireCell::WireVector wires() const {
//	return m_wires;
//    }
    };
} // namespace WireCell;


// Return a Ray going from the center point of wires[0] to a point on
// wire[1] and perpendicular to both.
static Ray pitch2(const std::vector<const IWire*>& wires)
{
    // Use two consecutive wires from the center to determine the pitch. 
    int ind = wires.size()/2;
    const IWire* one = wires[ind];
    const IWire* two = wires[ind+1];
    const Ray p = ray_pitch(one->ray(), two->ray());

    // Move the pitch to the first wire
    const IWire* zero = wires[0];
    const Vector center0 = zero->center();
    return Ray(center0, center0 + ray_vector(p));
}

static Vector axis(const std::vector<const IWire*>& wires)
{
    int ind = wires.size()/2;
    return ray_vector(wires[ind]->ray()).norm();
}

struct WireByIndex {
    bool operator() (const IWire* lhs, const IWire *rhs) const {
	if (lhs->plane() == rhs->plane()) {
	    return lhs->index() < rhs->index();
	}
	return lhs->plane() < lhs->plane();
    }
};

Vector origin_cross(const std::vector<const IWire*>& ones,
		    const std::vector<const IWire*>& twos) {
    Vector cross = ray_pitch(ones[0]->ray(), twos[0]->ray()).first;
    cross.x(0.0);
    return cross;
}


void BoundCells::generate(wire_iterator wires_begin, wire_iterator wires_end)
{
    m_store.clear();

    /* This was originally Xin's cell algorithm but only the concept
       remains.

       The idea is based on "crossing points" and "wire half-way
       lines".  Crossing points are the points in the Y-Z plane where
       lines which are parallel to that plane cross.  Wire half-way
       lines are the lines running parallel to a given wire and within
       the wire plane which are 1/2 the of a pitch distance away.

       The algorithm visits each U and V wire pair and determines
       their crossing point.  If this point is outside the bounding
       box of the wire planes it is discarded.  Otherwise, the four
       crossing points of each wire half-way lines are found.  Any
       that are inside the bounding box are saved as candidate cell
       corner points.

       Then, for each W wire, these four points are checked and any
       which are withing 1/2 pitch of the wire are kept as cell corner
       points.

       In addition the wire half-way crossing points for the W wire
       and the U wire are found and checked to be within the wire
       half-way distance with the current V wires.  This is repeated
       with U and V swapped.  Surviving points are added to the list
       of cell corner points.

    */
    
    vector<const IWire*> u_wires, v_wires, w_wires;
    copy_if(wires_begin, wires_end, back_inserter(u_wires), select_u_wires);
    copy_if(wires_begin, wires_end, back_inserter(v_wires), select_v_wires);
    copy_if(wires_begin, wires_end, back_inserter(w_wires), select_w_wires);

    WireByIndex wbi_sorter;
    std::sort(u_wires.begin(), u_wires.end(), wbi_sorter);
    std::sort(v_wires.begin(), v_wires.end(), wbi_sorter);
    std::sort(w_wires.begin(), w_wires.end(), wbi_sorter);

    cerr << "#wires: "
	 << u_wires.size() << ", "
	 << v_wires.size() << ", "
	 << w_wires.size() <<endl;

    const Ray pitch_u_ray = pitch2(u_wires);
    const Ray pitch_v_ray = pitch2(v_wires);
    const Ray pitch_w_ray = pitch2(w_wires);

    cerr << "pitche vectors:\n"
    	 << "\tU: " << pitch_u_ray.first << "-->" << pitch_u_ray.second << "\n"
    	 << "\tV: " << pitch_v_ray.first << "-->" << pitch_v_ray.second << "\n"
    	 << "\tW: " << pitch_w_ray.first << "-->" << pitch_w_ray.second << endl;

    const double pitch_u = ray_length(pitch_u_ray);
    const double pitch_v = ray_length(pitch_v_ray);
    const double pitch_w = ray_length(pitch_w_ray);
    

    const Vector axis_u = axis(u_wires);
    const Vector axis_v = axis(v_wires);
    const Vector axis_w = axis(w_wires);
    const Vector jump_u = axis_u * (pitch_u / sin(2.0 * acos(axis_w.dot(axis_u))));
    const Vector jump_v = axis_v * (pitch_v / sin(2.0 * acos(axis_w.dot(axis_v))));

    // jumps from a wire crossing point to the four half-wire crossing
    // points, in cyclical order.
    const Vector cross_jump_uv[4] = {
	+ 0.5*jump_u + 0.5*jump_v, // +Y/Z=0
	+ 0.5*jump_u - 0.5*jump_v, // Y=0/+Z
	- 0.5*jump_u - 0.5*jump_v, // -Y/Z=0
	- 0.5*jump_u + 0.5*jump_v  // Y=0/-Z
    };

    //const double tolerance = 0.1*units::mm;
    const double tolerance = 0.0; // fixme: generate noisy wires and test this value


    // pack it up and send it out
    std::vector<const IWire*> uvw_wires(3);

    const Vector origin_uv = origin_cross(u_wires, v_wires);

    for (int u_ind = 0; u_ind < u_wires.size(); ++u_ind) {
	const IWire& u_wire = *u_wires[u_ind];
	uvw_wires[0] = &u_wire;

	for (int v_ind = 0; v_ind < v_wires.size(); ++v_ind) {
	    const IWire& v_wire = *v_wires[v_ind];
	    uvw_wires[1] = &v_wire;

	    // source of precision loss?
	    Vector cross_uv = origin_uv + double(u_ind)*jump_v + double(v_ind)*jump_u;

            std::vector<Vector> puv(4);
            double dis_puv[4];
	    for (int ind=0; ind<4; ++ind) {
		puv[ind] = cross_uv + cross_jump_uv[ind];
	    }

	    // Prefilter W wires, only bother checking those nearby.
	    int max_w_ind = min(int((puv[1].z() + 0.5*pitch_w)/pitch_w), int(w_wires.size()-1));
	    int min_w_ind = max(int((puv[3].z() - 0.5*pitch_w)/pitch_w), 0);
	    for (int w_ind = min_w_ind; w_ind <= max_w_ind; ++w_ind) {
		const IWire& w_wire = *w_wires[w_ind];
		uvw_wires[2] = &w_wire;

		double target_w = w_ind * pitch_w;

                WireCell::PointVector pcell;
		vector<int> uv_ind;

		bool inside_uv[4];
		// check which of the four U/V crossings are near this W wire.
                for (int ind=0; ind<4; ++ind) {
		    if (fabs(target_w - puv[ind].z()) < 0.5*(pitch_w + tolerance)) {
			inside_uv[ind] = true;
			pcell.push_back(puv[ind]);
                    }
		    else {
			inside_uv[ind] = false;
		    }
                }

                if (!pcell.size()) {
		    // fixme: check if this fires, it shouldn't due to the pre-filtering of W wires
                    continue;	
                }

		vector<std::pair<int,int> > to_break;
		for (int ind=0; ind<4; ++ind) {
		    int other_ind = (ind+1)%4;
		    if (inside_uv[ind] && inside_uv[other_ind]) {
			continue;
		    }
		    if (inside_uv[ind]) {
			to_break.push_back(std::pair<int,int>(ind, other_ind));
		    }
		    else {
			to_break.push_back(std::pair<int,int>(other_ind, ind));
		    }
		}

		for (int ind = 0; ind < to_break.size(); ++ind) {
		    const Vector& p_in = puv[to_break[ind].first];
		    const Vector& p_out = puv[to_break[ind].second];
		    
		    double over_z = p_in.z() - target_w - 0.5*pitch_w;
		    Vector diff = p_out - p_in;
		    double rel = over_z / diff.z();
		    Vector corner = p_in + rel*diff;
		    pcell.push_back(corner);
		}
			
		// result
		m_store.push_back(new BoundCell(m_store.size(), pcell, uvw_wires));

            } // W wires
        } // v wires
    } // u wires
} // generate()



typedef WireCell::IteratorAdapter< std::vector<BoundCell*>::iterator, cell_base_iterator > bc_iterator;

WireCell::cell_iterator BoundCells::cells_begin()
{
    return bc_iterator(m_store.begin());
}


WireCell::cell_iterator BoundCells::cells_end()
{
    return bc_iterator(m_store.end());
}


BoundCells::BoundCells()
{
}

BoundCells::~BoundCells()
{
}

