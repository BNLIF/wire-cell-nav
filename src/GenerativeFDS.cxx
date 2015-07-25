#include "WireCellNav/GenerativeFDS.h"
#include "TMath.h"
#include "TRandom.h"

using namespace WireCell;
using namespace std;

double integral(double sigma, double z0, double z1, double z2){
  double result;
  result += 0.5 * TMath::Erf((z2-z0)/sqrt(2.)/sigma) 
    - 0.5 *TMath::Erf((z1-z0)/sqrt(2.)/sigma);
  return result;
}

GenerativeFDS::GenerativeFDS(const Depositor& dep, const GeomDataSource& gds, 
			     int bins_per_frame1, int nframes_total,
			     float bin_drift_distance)
    : dep(dep)
    , gds(gds)
    , max_frames(nframes_total)
    , bin_drift_distance(bin_drift_distance)
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
	
	// adding in the diffusion
	// assuming the velocity is 1.6 mm/us
	float drift_time = (pt.x-xmm.second)/(1.6*units::millimeter); // us
	float DL = 5; //cm^2/s
	float DT = 13; //cm^2/s
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
	  long_tbin.push_back(tt);
	  long_integral.push_back(integral(sigmaL,(pt.x-xmm.second),tt*bin_drift_distance,(tt+1)*bin_drift_distance));

	  tt = tbin + kk + 1;
	  long_tbin.push_back(tt);
	  long_integral.push_back(integral(sigmaL,(pt.x-xmm.second),tt*bin_drift_distance,(tt+1)*bin_drift_distance));
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
	    //cerr << "GenerativeFDS: drop: point not contained: " << pt << endl;
	    continue;
	}
	  
	WireCell::SimTruth st(pt.x, pt.y, pt.z, charge, tbin, simtruth.size());
	simtruth.insert(st);
	//cerr << "SimTruth: " << st.trackid() << " q=" << st.charge()
	//     << " tbin=" << tbin << " pos=" << st.pos() << endl;

	for (int iplane=0; iplane < 3; ++iplane) {
	    WirePlaneType_t plane = static_cast<WirePlaneType_t>(iplane); // annoying
	    const GeomWire* wire = gds.closest(pt, plane);
	    int chid = wire->channel();
	    int windex = wire->index();

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
		alltime.push_back(long_tbin.at(qt));
		allwires.push_back(trans_wires.at(qw));
		float tcharge = charge * long_integral.at(qt) * 
		  trans_integral.at(qw);
		tcharge = gRandom->Gaus(tcharge,sqrt(tcharge));
		allcharge.push_back(tcharge);
		sum_charge += tcharge;
	      }
	    }
	    //do normalization ... 
	    for (int qx = 0; qx!=allcharge.size();qx++){
	      const GeomWire* wire3 = allwires.at(qx);
	      chid = wire3->channel();
	      int tbin3 = alltime.at(qx);
	      float charge3 = allcharge.at(qx)/sum_charge;
	      
	      TraceIndexMap::iterator it = tim.find(chid);
	      int trace_index = frame.traces.size(); // if new
	      if (it == tim.end()) {
		Trace t;
		t.chid = chid;
		t.tbin = 0;
		t.charge.resize(bins_per_frame, 0.0);
		tim[chid] = frame.traces.size();
		frame.traces.push_back(t);
	      }else {		// already seen
		trace_index = it->second;
	      }
	      Trace& trace = frame.traces[trace_index];
	      
	      // finally
	      trace.charge[tbin3] += charge3;
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
    
    frame.index = frame_number; 
    return frame.index;
}

