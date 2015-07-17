#include "WireCellNav/BoundCells.h"

using namespace WireCell;

BoundCells::BoundCells()
{
}

BoundCells::~BoundCells()
{
}


// This is the actual implementation of the cell we provide.  It is
// buried in this implementation for a reason.  It's not a publicly
// accessible class.
class BoundCell : public ICell {
    
    int m_ident;
    PointVector m_corners;	// this is likely a source of a lot of memory usage
    WireVector m_wires;

public:
    BoundCell(int id, const PointVector& pcell, const WireVector& wires)
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

    virtual WireCell::WireVector wires() const {
	return m_wires;
    }
};


void BoundCells::generate(const WireCell::IWireDatabase& wdb)
{
    m_store.clear();

    // This is Xin's cell definition algorithm sung to the tune of
    // graph minor.

    const WireCell::WireVector& u_wires = wdb.wires_in_plane(kUwire);
    const WireCell::WireVector& v_wires = wdb.wires_in_plane(kVwire);
    const WireCell::WireVector& w_wires = wdb.wires_in_plane(kWwire);

    float pitch_u = wdb.pitch(kUwire);
    float pitch_v = wdb.pitch(kVwire);
    float pitch_w = wdb.pitch(kYwire);
    
    std::pair<int,int> box_ind[4] = { // allows loops over a box of indices
        std::pair<int,int>(0,0),
        std::pair<int,int>(0,1),
        std::pair<int,int>(1,1),
        std::pair<int,int>(1,0)
    };

    // pack it up and send it out
    WireVector uvw_wires(3);

    for (int u_ind=0; u_ind < u_wires.size(); ++u_ind) {
        WireCell::Wire u_wire = u_wires[u_ind];
	uvw_wires[0] = u_wire;
        float dis_u_wire = wdb.wire_dist(u_wire);
        float dis_u[2] = { dis_u_wire - pitch_u, dis_u_wire + pitch_u }; // half-line minmax

        for (int v_ind=0; v_ind < v_wires.size(); ++v_ind) {
            WireCell::Wire v_wire = v_wires[v_ind];
	    uvw_wires[1] = v_wire;
            float dis_v_wire = wdb.wire_dist(v_wire);
            float dis_v[2] = { dis_v_wire - pitch_v, dis_v_wire + pitch_v };


            std::vector<Vector> puv(4);
            float dis_puv[4];
            for (int ind=0; ind<4; ++ind) {
                // fixme: we are not handling the case where one
                // of these crossing points are outside the wire
                // plane boundary.
                wdb.crossing_point(dis_u[box_ind[ind].first], dis_v[box_ind[ind].second], kUwire, kVwire, puv[ind]);
                dis_puv[ind] = wdb.wire_dist(puv[ind], kYwire);
            }

            for (int w_ind=0; w_ind < w_wires.size(); ++w_ind) {
                WireCell::Wire w_wire = w_wires[w_ind];
		uvw_wires[2] = w_wire;
                float dis_w_wire = wdb.wire_dist(w_wire);
                float dis_w[2] = { dis_w_wire - pitch_w, dis_w_wire + pitch_w };

                WireCell::PointVector pcell;

                for (int ind=0; ind<4; ++ind) {
                    if (dis_w[0] <= dis_puv[ind] && dis_puv[ind] < dis_w[1]) {
                        pcell.push_back(puv[ind]);
                    }
                }

                if (!pcell.size()) { 
                    continue;
                }

                for (int ind=0; ind<4; ++ind) {
                    {           // fresh scope
                        WireCell::Vector pointvec;
                        if (wdb.crossing_point(dis_u[box_ind[ind].first], dis_w[box_ind[ind].second], 
                                               kUwire, kYwire, pointvec)) 
                            {
                                float disv = wdb.wire_dist(pointvec, kVwire);
                                if (dis_v[0] <= disv && disv < dis_v[1]) {
                                    pcell.push_back(pointvec);
                                }
                            }
                    }

                    {           // fresh scope
                        WireCell::Vector pointvec;
                        if (wdb.crossing_point(dis_v[box_ind[ind].first], dis_w[box_ind[ind].second], 
                                               kVwire, kYwire, pointvec)) 
                            {
                                float disu = wdb.wire_dist(pointvec, kUwire);
                                if (dis_u[0] <= disu && disu < dis_u[1]) {
                                    pcell.push_back(pointvec);
                                }
                            }
                    }

                }

		// result
		Cell thecell(new BoundCell(m_store.size(), pcell, uvw_wires));
		m_store.insert(thecell);

            } // W wires
        } // v wires
    } // u wires
} // generate()



const WireCell::CellSet& BoundCells::cells() const
{
    return m_store;
}
