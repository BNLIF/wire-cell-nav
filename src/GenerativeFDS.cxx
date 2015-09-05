#include "WireCellNav/GenerativeFDS.h"
#include "TMath.h"

using namespace WireCell;
using namespace std;

double integral(double sigma, double z0, double z1, double z2){
  double result=0;
  result += 0.5 * TMath::Erf((z2-z0)/sqrt(2.)/sigma) 
    - 0.5 *TMath::Erf((z1-z0)/sqrt(2.)/sigma);
  return result;
}

GenerativeFDS::GenerativeFDS(const Depositor& dep, const GeomDataSource& gds, 
			     int bins_per_frame1, int nframes_total,
			     float bin_drift_distance, float unit_dis)
    : dep(dep)
    , gds(gds)
    , max_frames(nframes_total)
    , bin_drift_distance(bin_drift_distance)
    , unit_dis(unit_dis)
{
  bins_per_frame = bins_per_frame1;
}


GenerativeFDS::~GenerativeFDS()
{
}

int GenerativeFDS::size() const { return max_frames; }

WireCell::SimTruthSelection GenerativeFDS::truth() const
{
    WireCell::SimTruthSelection ret;

    for (auto it = simtruth.begin(); it != simtruth.end(); ++it) {
	ret.push_back(& (*it));
    }
    return ret;
}



int GenerativeFDS::jump(int frame_number)
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
    if (!nhits) {
	frame.index = frame_number; 
	return frame.index;
    }

    typedef map<int, int> TraceIndexMap; // channel->index into traces;
    TraceIndexMap tim;		// keep tabs on what channels we've seen already

    std::pair<double, double> xmm = gds.minmax(0);
    
    for (size_t ind=0; ind<nhits; ++ind) {
	const Point& pt = hits[ind].first;
	float charge = hits[ind].second;
	int tbin = int((pt.x-xmm.second)/bin_drift_distance);
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
	float drift_time = (pt.x-xmm.second)/(unit_dis*units::millimeter); // us
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
	long_integral.push_back(integral(sigmaL,(pt.x-xmm.second),tbin*bin_drift_distance,(tbin+1)*bin_drift_distance));
	for (int kk =0; kk!=ntbin;kk++){
	  int tt = tbin - kk - 1;
	  if (tt >0){
	    long_tbin.push_back(tt);
	    long_integral.push_back(integral(sigmaL,(pt.x-xmm.second),tt*bin_drift_distance,(tt+1)*bin_drift_distance));
	  }
	  
	  tt = tbin + kk + 1;
	  if (tt < bins_per_frame){
	    long_tbin.push_back(tt);
	    long_integral.push_back(integral(sigmaL,(pt.x-xmm.second),tt*bin_drift_distance,(tt+1)*bin_drift_distance));
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

	if (!gds.contained_yz(pt)) {
	  //std::cout << "GenerativeFDS: drop: point not contained: " << pt.x/units::cm << " " << pt.y/units::cm  << " " << pt.z/units::cm << std::endl;
	    continue;
	}else{
	  // std::cout << "GenerativeFDS: drop: point contained: " << pt.x/units::cm << " " << pt.y/units::cm  << " " << pt.z/units::cm << std::endl;
	}
	  
	WireCell::SimTruth st(pt.x, pt.y, pt.z, charge, tbin, simtruth.size());
	simtruth.insert(st);
	//cerr << "SimTruth: " << st.trackid() << " q=" << st.charge()
	//     << " tbin=" << tbin << " pos=" << st.pos() << endl;

	for (int iplane=0; iplane < 3; ++iplane) {
	    WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane); // annoying
	    const GeomWire* wire = gds.closest(pt, plane);

	    

	    if (wire!=0){
	      int chid = wire->channel();
	      int windex = wire->index();
	    
	      //   std::cout << iplane << " " << chid << " " << windex << std::endl;
  
	      // start to do the transverse diffusion here ...
	      double pitch = gds.pitch(plane);
	      int nwbin = sigmaT*3/pitch + 1; // +- ntinb 3sigma
	      GeomWireSelection trans_wires;
	      std::vector<double> trans_integral;
	      double dist = gds.wire_dist(pt,plane);
	      double wdist = gds.wire_dist(*wire);
	      trans_wires.push_back(wire);
	      trans_integral.push_back(integral(sigmaT,dist,wdist-pitch/2.,wdist+pitch/2.));
	      //fill the rest of wires
	      for (int kk =0; kk!=nwbin;kk++){
		const GeomWire* wire1 = gds.by_planeindex(plane,windex-kk-1);
		if (wire1!=0){
		  wdist = gds.wire_dist(*wire1);
		  trans_wires.push_back(wire1);
		  trans_integral.push_back(integral(sigmaT,dist,wdist-pitch/2.,wdist+pitch/2.));
		}
		
		const GeomWire* wire2 = gds.by_planeindex(plane,windex+kk+1);
		if (wire2!=0){
		  wdist = gds.wire_dist(*wire2);
		  trans_wires.push_back(wire2);
		  trans_integral.push_back(integral(sigmaT,dist,wdist-pitch/2.,wdist+pitch/2.));
		}
	      }
	      // std::cout << sigmaT/units::mm << std::endl;
	      // for (int kk = 0; kk!=trans_integral.size();kk++){
	      //   std::cout << trans_integral.at(kk) << std::endl;
	      // }
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

    

    
    frame.index = frame_number; 
    return frame.index;
}

