#include "WireCellNav/WrappedGDS.h"
#include "TMath.h"
#include <cassert>
#include <iostream>		// debug
using namespace std;
using namespace WireCell;

// In the refactoring, using shared pointers is the norm, but here in
// the prototype code we keep it to ourselves.
typedef std::shared_ptr<Vector> VectorPtr;
typedef std::pair<VectorPtr, VectorPtr> VectorPtrPair;
typedef std::map<VectorPtr, VectorPtrPair> WirePointMesh;

namespace {
    static void get_points(std::vector<VectorPtr>& ret,
			   WirePointMesh& wm, std::set<VectorPtr>& end, VectorPtr point, int segment)
    {
        VectorPtrPair& vpp = wm[point];
	VectorPtr found;
	if (segment%2) {		// go v
	    found = vpp.second;
	}
	else {			// go u
	    found = vpp.first;
	}
	ret.push_back(found);
	if (end.find(found) == end.end()) { // not yet
	    get_points(ret, wm, end, found, segment+1);
	}
    }

    static unsigned int make_ident(int face, int top_ind, int uvw, int segment, int apa, int cryo)
    {
        //return (top_ind+1) + (segment+1)*1000 + (uvw+1)*10000 + (face+1)*100000;
	return (top_ind+1) + (segment+1)*1000 + (uvw+1)*10000 + (face+1)*100000 + (apa+1)*1000000 + (cryo+1)*100000000;
    }
  
    static int make_chan(int face, int top_ind, int uvw, int apa, int cryo)
    {
        //return (top_ind+1) +                   (uvw+1)*10000 + (face+1)*100000;
        return (top_ind+1) + (uvw+1)*1000 + (face+1)*10000 + (apa+1)*100000 + (cryo+1)*10000000;
    }

  //static int make_index(int face, int top_ind, int uvw, int segment)
  //{
  //    return (top_ind+1) + (segment+1)*1000 + (uvw+1)*10000 + (face+1)*100000;
  //}
}

void WireCell::WrappedGDS::uvw_wire_mesh(double angle, double pitch)
{
    const Vector drift(-1,0,0);

    // distance along wire between two consecutive u/v crossings
    const double stride = (pitch / sin(2*angle))*units::cm;

    // Vectors pointing in the unit direction of a U/V wire.  Both
    // point in the +Y direction.  U in the -Z and V in the +Z.
    const Vector wireU = stride * Vector(0, sin(-angle), cos(-angle));
    const Vector wireV = stride * Vector(0, sin(+angle), cos(+angle));

    // Vectors that connect adjacent wire crossings/attachments in
    // either the horizontal (Z) or vertical (Y) directions.
    const Vector horz_step = wireV - wireU;
    const double horz_step_mag = horz_step.magnitude();
    const Vector vert_step = wireV + wireU;
    const double vert_step_mag = vert_step.magnitude();

    // integral number of horizontal/vertical steps that fit in each
    // direction of the bounding box.
    int n_horz_steps = int((m_maxbound.z - m_minbound.z)*units::cm/horz_step_mag);
    int n_vert_steps = int((m_maxbound.y - m_minbound.y)*units::cm/vert_step_mag);

    //cerr << n_horz_steps << " horizontal steps" << endl;
    //cerr << n_vert_steps << " vertical steps" << endl;
    
    // calculate new bounds
    const Vector center_bb = 0.5*(m_maxbound+ m_minbound);
    const double x_delta = (m_maxbound.x - m_minbound.x)*units::cm / 8.0;

    const double y_min = center_bb.y*units::cm - 0.5*n_vert_steps*vert_step_mag;
    const double y_max = y_min + n_vert_steps*vert_step_mag;
    const double z_min = center_bb.z*units::cm - 0.5*n_horz_steps*horz_step_mag;
    const double z_max = z_min + n_horz_steps*horz_step_mag;

    // fill attachment points along left and right hand sides
    std::vector<VectorPtr> lhs, rhs;
    for (double y_val = y_min+0.5*vert_step_mag; y_val <= y_max; y_val += vert_step_mag) {
	lhs.push_back(VectorPtr(new Vector(0, y_val, z_min)));
	rhs.push_back(VectorPtr(new Vector(0, y_val, z_max)));
    }
    // fill attachment points along top and bottom
    std::vector<VectorPtr> top, bot;
    for (double z_val = z_min+0.5*horz_step_mag; z_val <= z_max; z_val += horz_step_mag) {
	bot.push_back(VectorPtr(new Vector(0, y_min, z_val)));
	top.push_back(VectorPtr(new Vector(0, y_max, z_val)));
    }

    int npoints = lhs.size() + bot.size();
    assert(npoints);

    // U wire attachment points going right-then-up
    std::vector<VectorPtr> u_ru;
    u_ru.insert(u_ru.end(), bot.begin(), bot.end());
    u_ru.insert(u_ru.end(), rhs.begin(), rhs.end());

    // U wire attachment points going up-then-right
    std::vector<VectorPtr> u_ur;
    u_ur.insert(u_ur.end(), lhs.begin(), lhs.end());
    u_ur.insert(u_ur.end(), top.begin(), top.end());

    // V wire attachment points going left-then-up
    std::vector<VectorPtr> v_lu;
    v_lu.insert(v_lu.end(), bot.rbegin(), bot.rend());
    v_lu.insert(v_lu.end(), lhs.begin(),  lhs.end());

    // V wire attachment points going up-the-left
    std::vector<VectorPtr> v_ul;
    v_ul.insert(v_ul.end(), rhs.begin(),  rhs.end());
    v_ul.insert(v_ul.end(), top.rbegin(), top.rend());

    // cerr << "#ru: " << u_ru.size() << endl;
    // cerr << "#ur: " << u_ur.size() << endl;
    // cerr << "#lu: " << v_lu.size() << endl;
    // cerr << "#ul: " << v_ul.size() << endl;

    map<VectorPtrPair, int> pair2ind;
    for (int ind=0; ind<npoints; ++ind) {
	pair2ind[VectorPtrPair(u_ur[ind],u_ru[ind])] = ind;
	pair2ind[VectorPtrPair(u_ru[ind],u_ur[ind])] = ind;
	pair2ind[VectorPtrPair(v_ul[ind],v_lu[ind])] = ind;
	pair2ind[VectorPtrPair(v_lu[ind],v_ul[ind])] = ind;
    }

    WirePointMesh wm;
    for (int ind=0; ind<npoints; ++ind) {
	wm[u_ru[ind]] = VectorPtrPair(u_ur[ind], 0);
	wm[u_ur[ind]] = VectorPtrPair(u_ru[ind], 0);
    }
    for (int ind=0; ind<npoints; ++ind) {
	wm[v_lu[ind]].second = v_ul[ind];
	wm[v_ul[ind]].second = v_lu[ind];
    }

    std::set<VectorPtr> bot_set(bot.begin(), bot.end());
    for (int top_ind=0; top_ind < top.size(); ++top_ind) {

	VectorPtr starting_point = top[top_ind];
	//cerr << "START" << top_ind << " " << *(starting_point.get()) <<endl;

	// starting u or v
	for (int starting_uv = 0; starting_uv < 2; ++starting_uv) {

	    vector<VectorPtr> points;
	    points.push_back(starting_point);
	    get_points(points, wm, bot_set, starting_point, starting_uv);
	    //cerr << "GOT " << points.size() << " points for uv=" << starting_uv << endl;
	    
	    for (int ipt=0; ipt<points.size()-1; ++ipt) {
		int segment = ipt;

		int this_uv = (starting_uv + ipt)%2;

		for (int this_face = 0; this_face < 2; ++this_face) {
		    
		    int this_face_uv = (this_uv+this_face)%2;

		    double this_x_val = x_delta*3.0; // u
		    if (this_face_uv) {
			this_x_val = x_delta*2.0; // v
		    }
		    if (this_face) { // "B" face
			this_x_val *= -1;
		    }

		    int ident = make_ident(this_face, top_ind, this_face_uv, segment, 0, 0);
		    int chan = make_chan(this_face, top_ind, this_face_uv, 0, 0);
		    //int index = make_index(this_face, top_ind, this_face_uv, segment);

		    Point p1(*(points[ipt+0].get()));
		    Point p2(*(points[ipt+1].get()));
		    p1.x = p2.x = this_x_val;

		    GeomWire gw(ident,
				WirePlaneType_t((((starting_uv + ipt)%2) + this_face)%2),
				pair2ind[VectorPtrPair(points[ipt],points[ipt+1])],
				//index,
				chan,
				p1, p2,
				segment, this_face, 0, 0);

		    //cerr << gw << " " << gw.point1() << " " << gw.point2() << endl;
		    this->add_wire(gw);
		}
	    }
	}
    }

    // W wires	
    for (int ind=0; ind < top.size(); ++ind) {
	VectorPtr top_point = top[ind];
	VectorPtr bot_point = bot[ind];

	for (int this_face = 0; this_face < 2; ++this_face) {

	    double this_x_val = x_delta; // u
	    if (this_face) { // "B" face
		this_x_val *= -1;
	    }
    
	    int ident = make_ident(this_face, ind, 2, 0, 0, 0);
	    int chan = make_chan(this_face, ind, 2, 0, 0);

	    Point p1(*top_point.get());
	    Point p2(*bot_point.get());
	    p1.x = p2.x = this_x_val;

	    GeomWire gw(ident,
			kYwire,
			ind,
			chan, p1, p2, 0, this_face, 0, 0);
	    this->add_wire(gw);
	}
    }
}

void WireCell::WrappedGDS::uv_wire_mesh(double angle, double pitch, bool is_u)
{
    //const Vector drift(-1,0,0);
    //std::cout<<"wire angle is "<<angle<<"; wire pitch is "<<pitch<<std::endl;
    // distance along wire between two consecutive u/v crossings
    double angle_in_radian = angle * TMath::Pi() / 180.;
    const double stride = (pitch / sin(2*angle_in_radian))*units::cm;

    // Vectors pointing in the unit direction of a U/V wire.  Both
    // point in the +Y direction.  U in the -Z and V in the +Z.
    const Vector wireU = stride * Vector(0, sin(-angle_in_radian), cos(-angle_in_radian));
    const Vector wireV = stride * Vector(0, sin(+angle_in_radian), cos(+angle_in_radian));

    // Vectors that connect adjacent wire crossings/attachments in
    // either the horizontal (Z) or vertical (Y) directions.
    const Vector horz_step = wireV - wireU;
    const double horz_step_mag = horz_step.magnitude();
    const Vector vert_step = wireV + wireU;
    const double vert_step_mag = vert_step.magnitude();

    // integral number of horizontal/vertical steps that fit in each
    // direction of the bounding box.
    int n_horz_steps = int((m_maxbound.z - m_minbound.z)*units::cm/horz_step_mag);
    int n_vert_steps = int((m_maxbound.y - m_minbound.y)*units::cm/vert_step_mag);

    //cerr << n_horz_steps << " horizontal steps" << endl;
    //cerr << n_vert_steps << " vertical steps" << endl;
    
    // calculate new bounds
    const Vector center_bb = 0.5*(m_maxbound + m_minbound);
    const double x_delta = ((m_maxbound.x - m_minbound.x) / 6.0) * units::cm;
    std::cout<<"center is ("<<center_bb.x<<","<<center_bb.y<<","<<center_bb.z<<")\t"
	     <<"x_delta is "<<x_delta<<std::endl;
    
    const double y_min = center_bb.y*units::cm - 0.5*n_vert_steps*vert_step_mag;
    const double y_max = y_min + n_vert_steps*vert_step_mag;
    const double z_min = center_bb.z*units::cm - 0.5*n_horz_steps*horz_step_mag;
    const double z_max = z_min + n_horz_steps*horz_step_mag;

    // fill attachment points along left and right hand sides
    std::vector<VectorPtr> lhs, rhs;
    for (double y_val = y_min+0.5*vert_step_mag; y_val <= y_max; y_val += vert_step_mag) {
	lhs.push_back(VectorPtr(new Vector(0, y_val, z_min)));
	rhs.push_back(VectorPtr(new Vector(0, y_val, z_max)));
    }
    // fill attachment points along top and bottom
    std::vector<VectorPtr> top, bot;
    for (double z_val = z_min+0.5*horz_step_mag; z_val <= z_max; z_val += horz_step_mag) {
	bot.push_back(VectorPtr(new Vector(0, y_min, z_val)));
	top.push_back(VectorPtr(new Vector(0, y_max, z_val)));
    }

    int npoints = lhs.size() + bot.size();
    assert(npoints);

    // U wire attachment points going right-then-up
    std::vector<VectorPtr> u_ru;
    u_ru.insert(u_ru.end(), bot.begin(), bot.end());
    u_ru.insert(u_ru.end(), rhs.begin(), rhs.end());

    // U wire attachment points going up-then-right
    std::vector<VectorPtr> u_ur;
    u_ur.insert(u_ur.end(), lhs.begin(), lhs.end());
    u_ur.insert(u_ur.end(), top.begin(), top.end());

    // V wire attachment points going left-then-up
    std::vector<VectorPtr> v_lu;
    v_lu.insert(v_lu.end(), bot.rbegin(), bot.rend());
    v_lu.insert(v_lu.end(), lhs.begin(),  lhs.end());

    // V wire attachment points going up-the-left
    std::vector<VectorPtr> v_ul;
    v_ul.insert(v_ul.end(), rhs.begin(),  rhs.end());
    v_ul.insert(v_ul.end(), top.rbegin(), top.rend());

    // cerr << "#ru: " << u_ru.size() << endl;
    // cerr << "#ur: " << u_ur.size() << endl;
    // cerr << "#lu: " << v_lu.size() << endl;
    // cerr << "#ul: " << v_ul.size() << endl;

    map<VectorPtrPair, int> pair2ind;
    for (int ind=0; ind<npoints; ++ind) {
	pair2ind[VectorPtrPair(u_ur[ind],u_ru[ind])] = ind;
	pair2ind[VectorPtrPair(u_ru[ind],u_ur[ind])] = ind;
	pair2ind[VectorPtrPair(v_ul[ind],v_lu[ind])] = ind;
	pair2ind[VectorPtrPair(v_lu[ind],v_ul[ind])] = ind;
    }

    WirePointMesh wm;
    for (int ind=0; ind<npoints; ++ind) {
	wm[u_ru[ind]] = VectorPtrPair(u_ur[ind], 0);
	wm[u_ur[ind]] = VectorPtrPair(u_ru[ind], 0);
    }
    for (int ind=0; ind<npoints; ++ind) {
	wm[v_lu[ind]].second = v_ul[ind];
	wm[v_ul[ind]].second = v_lu[ind];
    }

    std::set<VectorPtr> bot_set(bot.begin(), bot.end());
    for (int top_ind=0; top_ind < top.size(); ++top_ind) {

	VectorPtr starting_point = top[top_ind];
	//cerr << "START" << top_ind << " " << *(starting_point.get()) <<endl;

	// starting u or v
	for (int starting_uv = 0; starting_uv < 2; ++starting_uv) {

	    vector<VectorPtr> points;
	    points.push_back(starting_point);
	    get_points(points, wm, bot_set, starting_point, starting_uv);
	    //cerr << "GOT " << points.size() << " points for uv=" << starting_uv << endl;
	    
	    for (int ipt=0; ipt<points.size()-1; ++ipt) {
		int segment = ipt;

		int this_uv = (starting_uv + ipt)%2;

		for (int this_face = 0; this_face < 2; ++this_face) {
		    
		    int this_face_uv = (this_uv+this_face)%2; // 0 = u; 1 = v

		    double this_x_val = 0.;
		    if ((!this_face_uv) && is_u) {
		        this_x_val = x_delta*3.0; // u
		    } else if (this_face_uv && (!is_u)) {
			this_x_val = x_delta*2.0; // v
		    } else {
		        continue;
		    }
		    if (this_face) { // "B" face
		        this_x_val *= -1.;
		    }

		    int ident = make_ident(this_face, top_ind, this_face_uv, segment, _apa, _cryo);
		    int chan = make_chan(this_face, top_ind, this_face_uv, _apa, _cryo);
		    //int index = make_index(this_face, top_ind, this_face_uv, segment);
		    //int index = pair2ind[VectorPtrPair(points[ipt],points[ipt+1])]*2+(int)this_face;
		    Point p1(*(points[ipt+0].get()));
		    Point p2(*(points[ipt+1].get()));
		    p1.x = p2.x = this_x_val + center_bb.x;

		    GeomWire gw(ident,
				//WirePlaneType_t((((starting_uv + ipt)%2) + this_face)%2),
				WirePlaneType_t(this_face_uv),
				pair2ind[VectorPtrPair(points[ipt],points[ipt+1])],
				//index,
				chan,
				p1, p2,
				segment, this_face, _apa, _cryo);

		    //cerr << gw << " " << gw.point1() << " " << gw.point2() << endl;
		    this->add_wire(gw);
		}
	    }
	}
    }
}


void WireCell::WrappedGDS::w_wire_mesh(double pitch)
{

    const double horz_step_mag = pitch*units::cm;
    int n_horz_steps = int((m_maxbound.z - m_minbound.z)*units::cm/horz_step_mag);

    // calculate new bounds
    const Vector center_bb = 0.5*(m_maxbound+ m_minbound);
    const double x_delta = ((m_maxbound.x - m_minbound.x) / 6.0) * units::cm;

    const double z_min = center_bb.z*units::cm - 0.5*n_horz_steps*horz_step_mag;
    const double z_max = z_min + n_horz_steps*horz_step_mag;

    // fill attachment points along top and bottom
    std::vector<VectorPtr> top, bot;
    for (double z_val = z_min+0.5*horz_step_mag; z_val <= z_max; z_val += horz_step_mag) {
	bot.push_back(VectorPtr(new Vector(0, m_minbound.y, z_val)));
	top.push_back(VectorPtr(new Vector(0, m_maxbound.y, z_val)));
    }

    // W wires	
    for (int ind=0; ind < top.size(); ++ind) {
	VectorPtr top_point = top[ind];
	VectorPtr bot_point = bot[ind];

	for (int this_face = 0; this_face < 2; ++this_face) {

	    double this_x_val = x_delta; // u
	    if (this_face) { // "B" face
		this_x_val *= -1;
	    }
    
	    int ident = make_ident(this_face, ind, 2, 0, _apa, _cryo);
	    int chan = make_chan(this_face, ind, 2, _apa, _cryo);
	    //int index = ind*2+(int)this_face;
	    
	    Point p1(*top_point.get());
	    Point p2(*bot_point.get());
	    p1.x = p2.x = this_x_val + center_bb.x;

	    GeomWire gw(ident,
			kYwire,
			ind,
			//index,
			chan, p1, p2, 0, this_face, _apa, _cryo);
	    this->add_wire(gw);
	}
    }
}


WireCell::WrappedGDS::WrappedGDS(const Vector& minbound, const Vector& maxbound, 
				 double angle, double pitch)
    : GeomDataSource()
    , m_minbound(minbound)
    , m_maxbound(maxbound)
{
    uvw_wire_mesh(angle, pitch);
}

WireCell::WrappedGDS::WrappedGDS(const Vector& minbound, const Vector& maxbound,
				 double angleU, double angleV, double pitch,
				 short cryo, short apa)
  : GeomDataSource()
  , m_minbound(minbound)
  , m_maxbound(maxbound)
  , _cryo(cryo)
  , _apa(apa)
{
    uv_wire_mesh(angleU, pitch, true);
    uv_wire_mesh(angleV, pitch, false);
    w_wire_mesh(pitch);
}

WireCell::WrappedGDS::~WrappedGDS()
{
}
