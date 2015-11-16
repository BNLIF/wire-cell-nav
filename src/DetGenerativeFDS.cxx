#include "WireCellNav/DetGenerativeFDS.h"
#include "TMath.h"

using namespace WireCell;
using namespace std;

namespace {
  double integral(double sigma, double z0, double z1, double z2){
    double result=0;
    result += 0.5 * TMath::Erf((z2-z0)/sqrt(2.)/sigma) 
      - 0.5 *TMath::Erf((z1-z0)/sqrt(2.)/sigma);
    return result;
  }
}

DetGenerativeFDS::DetGenerativeFDS(const Depositor& dep, const DetectorGDS& gds, 
				   int bins_per_frame1, int nframes_total,
				   float bin_drift_distance, float unit_dis)
    : dep(dep)
    , det_gds(gds)
    , max_frames(nframes_total)
    , bin_drift_distance(bin_drift_distance)
    , unit_dis(unit_dis)
{
  bins_per_frame = bins_per_frame1;
}


DetGenerativeFDS::~DetGenerativeFDS()
{
}

int DetGenerativeFDS::size() const { return max_frames; }

WireCell::SimTruthSelection DetGenerativeFDS::truth() const
{
    WireCell::SimTruthSelection ret;

    for (auto it = simtruth.begin(); it != simtruth.end(); ++it) {
	ret.push_back(& (*it));
    }
    return ret;
}

int DetGenerativeFDS::jump(int frame_number)
{
    if (frame_number < 0) {
	return -1;
    }
    if (max_frames >=0 && frame_number >= max_frames) {
	return -1;
    }

    if (frame.index == frame_number) {
	return frame_number;
    }

    frame.clear();
    simtruth.clear();

    const PointValueVector& hits = dep.depositions(frame_number);
    const std::vector<int>& timeoffsets = dep.timeoffset();
    
    size_t nhits = hits.size();
    //std::cout<<"nhits = "<<nhits<<std::endl;
    if (!nhits) {
	frame.index = frame_number; 
	return frame.index;
    }

    //std::cout<<"frame.index != frame_number"<<std::endl;
    
    typedef map<int, int> TraceIndexMap; // channel->index into traces;
    TraceIndexMap tim;		// keep tabs on what channels we've seen already

    std::pair<double, double> xmm;
    
    //std::cout << nhits << std::endl;

    const WrappedGDS *apa_gds = 0;

    for (size_t ind=0; ind<nhits; ++ind) {
	const Point& pt = hits[ind].first;
	//const Vector vpt(pt.x, pt.y, pt.z);
	float charge = hits[ind].second;
	// decide which apa the charge deposit belongs to
	short which_cryo = det_gds.in_which_cryo(pt);
	short which_apa = det_gds.in_which_apa(pt);
	//std::cout<<"point ("<<pt.x<<", "<<pt.y<<", "<<pt.z<<") is in the "<<which_cryo<<"th "
	//	 <<"cryo and the "<<which_apa<<"th apa"<<std::endl;
	apa_gds = det_gds.get_apaGDS(det_gds.in_which_cryo(pt), det_gds.in_which_apa(pt));
	if (apa_gds==NULL) {
	  // out side the detector boundary ... 
	  //  std::cout<<"exit because apa_gds = NULL"<<std::endl; 
	  continue;
	}
	xmm = apa_gds->minmax(0); 

	//	std::cout << pt.x/units::cm << " " << xmm.first/units::cm << " " << xmm.second/units::cm << std::endl;
	if (pt.x>xmm.first && pt.x<xmm.second) continue;// space in between wires in an APA are treated as dead region
	
	int face = 0;
	float drift_dist;
	
	if (TMath::Abs(pt.x-xmm.first) > TMath::Abs(pt.x-xmm.second)) {
	  drift_dist = TMath::Abs(pt.x-xmm.second);
	  face = 1;
	}else{
	  drift_dist = TMath::Abs(pt.x-xmm.first);
	}
	// float drift_dist = TMath::Abs(pt.x-xmm.first) < TMath::Abs(pt.x-xmm.second) ? TMath::Abs(pt.x-xmm.first) : TMath::Abs(pt.x-xmm.second);
	int tbin = int(drift_dist/bin_drift_distance);
	tbin = TMath::Abs(tbin);
	int offset;
	//std::cout << timeoffsets.size() << std::endl;
	if (timeoffsets.size()==0){
	  offset = 0;
	}else{
	  offset = timeoffsets[ind];
	}
	//std::cout << tbin << " " << pt.x/units::cm << " " << xmm.second/units::cm << " " << offset << std::endl;

	// adding in the diffusion
	// assuming the velocity is 1.6 mm/us
	float drift_time = drift_dist/(unit_dis*units::millimeter); // us


	// can not handle negative drift time yet .... 
	if (drift_time < 0) {
	  //std::cout<<"exit because drift time < 0"<<std::endl; 
	  continue;
	}
	
	float DL = 5.3; //cm^2/s
	float DT = 12.8; //cm^2/s
	float sigmaL = sqrt(2.*DL*drift_time*1e-6) * units::cm;
	float sigmaT = sqrt(2.*DT*drift_time*1e-6) * units::cm;
	
	// do longitudinal array first
	int ntbin = sigmaL*3/bin_drift_distance + 1; // +- ntinb 3sigma
	std::vector<int> long_tbin;
	std::vector<double> long_integral;
	//push middle bin first
	long_tbin.push_back(tbin);
	long_integral.push_back(integral(sigmaL,drift_dist,tbin*bin_drift_distance,(tbin+1)*bin_drift_distance));


	for (int kk =0; kk!=ntbin;kk++){
	  int tt = tbin - kk - 1;
	  if (tt >0){
	    long_tbin.push_back(tt);
	    long_integral.push_back(integral(sigmaL,drift_dist,tt*bin_drift_distance,(tt+1)*bin_drift_distance));
	  }
	  
	  tt = tbin + kk + 1;
	  if (tt < bins_per_frame){
	    long_tbin.push_back(tt);
	    long_integral.push_back(integral(sigmaL,drift_dist,tt*bin_drift_distance,(tt+1)*bin_drift_distance));
	  }
	}

	// std::cout << sigmaL/units::mm << std::endl;
	// for (int kk = 0; kk!=long_tbin.size();kk++){
	//   std::cout << long_tbin.at(kk) << " " << long_integral.at(kk) << std::endl;
	// }
	// std::cout << std::endl;
	// end of longitudinal diffusion calculation ... 


	if (tbin >= bins_per_frame) {
	  //cerr << "GenerativeFDS: drop: tbin=" << tbin << " >= " << bins_per_frame << endl;
	    continue;
	}

	// if (!apa_gds->contained_yz(pt)) {
	//   //std::cout << "GenerativeFDS: drop: point not contained: " << pt.x/units::cm << " " << pt.y/units::cm  << " " << pt.z/units::cm << std::endl;
	//     continue;
	// }else{
	//   // std::cout << "GenerativeFDS: drop: point contained: " << pt.x/units::cm << " " << pt.y/units::cm  << " " << pt.z/units::cm << std::endl;
	// }
	  
	WireCell::SimTruth st(pt.x, pt.y, pt.z, charge, tbin, simtruth.size());
	simtruth.insert(st);
	//cerr << "SimTruth: " << st.trackid() << " q=" << st.charge()
	//     << " tbin=" << tbin << " pos=" << st.pos() << endl;

	for (int iplane=0; iplane < 3; ++iplane) {
	  WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane); // annoying
	  // only look up wires from the correct face
	  const GeomWire* wire = apa_gds->closest(pt, plane, face);

	  //if (wire->face() != face) continue;

	  if (wire!=0){
	    int chid = wire->channel();
	    int windex = wire->index();
	    //std::cout << iplane << " " << chid << " " << windex << std::endl;
  
	    // start to do the transverse diffusion here ...
	    double pitch = apa_gds->pitch(plane);
	    double angle = apa_gds->angle(plane);
	    //const Point shift(0, pitch*std::sin(angle), -pitch*std::cos(angle));
	    int nwbin = sigmaT*3/pitch + 1; // +- ntinb 3sigma
	    GeomWireSelection trans_wires;
	    std::vector<double> trans_integral;
	    double dist = apa_gds->wire_dist(pt,plane);
	    double wdist = apa_gds->wire_dist(*wire);
	    trans_wires.push_back(wire);
	    trans_integral.push_back(integral(sigmaT,dist,wdist-pitch/2.,wdist+pitch/2.));
	    //fill the rest of wires
	    for (int kk =0; kk!=nwbin;kk++){
	      // const Point shift1(shift.x, shift.y*(kk+1.), shift.z*(kk+1.));
	      //const Point& pt1 = pt - shift1;
	      //const GeomWire* wire1 = apa_gds->closest(pt1, plane, face);
	      const GeomWire* wire1 = apa_gds->get_ad_wire(wire,kk+1,plane,face);
	      if (wire1!=0){
	      	wdist = apa_gds->wire_dist(*wire1);
	      	trans_wires.push_back(wire1);
	      	trans_integral.push_back(integral(sigmaT,dist,wdist-pitch/2.,wdist+pitch/2.));
	      }
	      
	//       //const GeomWire* wire2 = apa_gds->by_planeindex(plane,windex+kk+1);
	//       const Point& pt2 = pt + shift1;
	//       const GeomWire* wire2 = apa_gds->closest(pt2, plane, face);
	      const GeomWire* wire2 = apa_gds->get_ad_wire(wire,-1-kk,plane,face);
	      if (wire2!=0){
	 	wdist = apa_gds->wire_dist(*wire2);
	 	trans_wires.push_back(wire2);
	 	trans_integral.push_back(integral(sigmaT,dist,wdist-pitch/2.,wdist+pitch/2.));
	      }
	    }
	  
	    //std::cout << sigmaT/units::mm << std::endl;
	    //for (int kk = 0; kk!=trans_integral.size();kk++){
	    //  std::cout << trans_integral.at(kk) << std::endl;
	    //}
	    // std::cout << std::endl;
	    // finish the calculation of transverse diffusion
	    
	    
	    // Now put everything into 1D arrays
	    // 1D wires
	    // 1D time
	    // 1D number of electrons with random
	    GeomWireSelection allwires;
	    std::vector<int> alltime;
	    std::vector<float> allcharge;
	    float sum_charge = 0;
	    for (int qt = 0; qt!= long_tbin.size(); qt++){
	      for (int qw = 0; qw!= trans_wires.size(); qw++){
		alltime.push_back(long_tbin.at(qt) + offset);
		allwires.push_back(trans_wires.at(qw));
		float tcharge = charge * long_integral.at(qt) * 
		  trans_integral.at(qw);
		allcharge.push_back(tcharge);
		sum_charge += tcharge;
	      }
	    }
	    
	    // for (int qx = 0; qx!=allcharge.size();qx++){
	    //   const GeomWire* wire3 = allwires.at(qx);
	    //   int chid3 = wire3->channel();
	    //   std::cout << alltime.at(qx) << " " << charge << " " << 
	    // 	allcharge.at(qx) << " " << chid3 << std::endl;
	    // }
	    // std::cout << std::endl;
	    
	    //do normalization ... 
	    for (int qx = 0; qx!=allcharge.size();qx++){
	      const GeomWire* wire3 = allwires.at(qx);
	      int chid3 = wire3->channel();
	      int tbin3 = alltime.at(qx);
	      
	      if (tbin3 >=0 && tbin3 < bins_per_frame){
		
		float charge3 = allcharge.at(qx)/sum_charge*charge;
		
		TraceIndexMap::iterator it = tim.find(chid3);
		int trace_index = frame.traces.size(); // if new
		if (it == tim.end()) {
		  Trace t;
		  t.chid = chid3;
		  t.tbin = 0;
		  t.charge.resize(bins_per_frame, 0.0);
		  tim[chid3] = frame.traces.size();
		  frame.traces.push_back(t);
		}else {		// already seen
		  trace_index = it->second;
		}
		Trace& trace = frame.traces[trace_index];
		
		// finally
		
		

		trace.charge[tbin3] += charge3;
	      }
	    }
	    
	    // TraceIndexMap::iterator it = tim.find(chid);
	    // int trace_index = frame.traces.size(); // if new
	    // if (it == tim.end()) {
	    // 	Trace t;
	    // 	t.chid = chid;
	    // 	t.tbin = 0;
	    // 	t.charge.resize(bins_per_frame, 0.0);
	    // 	tim[chid] = frame.traces.size();
	    // 	frame.traces.push_back(t);
	    // }
	    // else {		// already seen
	    // 	trace_index = it->second;
	    // }
	    // Trace& trace = frame.traces[trace_index];
	    // // finally
	    // trace.charge[tbin] += charge;
	  }
	}	
    }

    
    //std::cout << "End!" << std::endl;
    
    frame.index = frame_number; 
    return frame.index;
}

