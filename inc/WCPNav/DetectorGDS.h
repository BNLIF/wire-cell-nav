#ifndef WIRECELLNAV_DETECTORDATASOURCE_H
#define WIRECELLNAV_DETECTORDATASOURCE_H

#include "WCPNav/WrappedGDS.h"
#include "WCPNav/GeomDataSource.h"

#include "TMath.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>

namespace WCP {

  class DetectorGDS {// : public WCP::GeomDataSource {
    
  public:
    DetectorGDS();
    DetectorGDS(std::vector<std::string> geometry);
    //DetectorGDS(int cryo, std::string geometry);
    virtual ~DetectorGDS();

  public:

    void read_geometry(std::vector<std::string> filename);
    void set_ncryos(short ncryo);
    void set_napas(short cryo, short napas);
    // configure all apas in all cryostats in one go
    void set_apas(std::vector<short> napa, std::vector<double> angleU, std::vector<double> angleV, std::vector<double> pitchU, std::vector<double> pitchV, std::vector<double> pitchW, std::vector<std::vector<Vector> > center, std::vector<std::vector<Vector> > halves);
    // configure each apa individually
    void set_apa(short cryo, short apa, double angleU, double angleV, double pitchU, double pitchV, double pitchW, Vector center, Vector halves);
    // to be called after apa configuration
    void buildGDS();

    const WrappedGDS* get_apaGDS(short cryo, short apa) const;
    void set_apaGDS(short cryo, short apa, const WrappedGDS* apaGDS);
    
    short ncryos() const {return _ncryos;}
    short napa(short cryo) const {return _napas.at(cryo);}

    short in_which_apa(const Vector& point) const;
    short in_which_cryo(const Vector& point) const;

    double get_angle(short cryo, WirePlaneType_t plane = kUnknownWirePlaneType) const;
    double get_pitch(short cryo, WirePlaneType_t plane = kUnknownWirePlaneType) const;
    bool crossing_point(double dist1, double dist2, const GeomWire& wire1, const GeomWire& wire2, Vector& result) const;
    const GeomWire* closest(const Vector& point,
			    WirePlaneType_t plane = kUnknownWirePlaneType,
			    int face = -999) const;

    // what do we need from this class?
    // for each wire, it has a face, apa_no, and cryo_no 
    // find the wires corresponding to this channel
    const GeomWireSelection& by_channel(int channel) const;
    
    int get_total_nwires(WirePlaneType_t plane) const;
    
    void build_channel_map();
    int channel_count(WirePlaneType_t plane) const;
    WirePlaneType_t channel_plane_conv(int channel) const;
    int channel_count_conv(int channel) const;
    /*
    bool contained(const Vector& point) const;
    bool contained_yz(const Vector& point) const;
    short contained_cryo(const Vector& point) const;
    short contained_apa(short cryo, const Vector& point) const;
    short contained_yz_cryo(const Vector& point) const;
    short contained_yz_apa(short cryo, const Vector& point) const;
    */
    Vector get_maxbound(short c, short a) {return _max_bound.at(c).at(a);}
    Vector get_minbound(short c, short a) {return _min_bound.at(c).at(a);} 
  private:

    mutable std::vector<std::vector<const WrappedGDS*> > _APAgds;
    
    short _ncryos;
    std::vector<short> _napas;

    std::vector<std::vector<Vector> > _max_bound; // [cryo][apa]
    std::vector<std::vector<Vector> > _min_bound; // [cryo][apa]
    std::vector<std::vector<Vector> > _center;
    std::vector<std::vector<Vector> > _halves;
    std::vector<double> _angleU, _angleV, _pitchU, _pitchV, _pitchW;
    
    std::map<int, int> _channel_umap;
    std::map<int, int> _channel_vmap;
    std::map<int, int> _channel_wmap;
  };

}

#endif
