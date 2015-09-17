#ifndef WIRECELLNAV_DETECTORDATASOURCE_H
#define WIRECELLNAV_DETECTORDATASOURCE_H

#include "WireCellNav/WrappedGDS.h"

#include "TMath.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace WireCell {

  class DetectorGDS : public WireCell::GeomDataSource {
    
  public:
    DetectorGDS();
    DetectorGDS(std::vector<std::string> geometry);
    virtual ~DetectorGDS();

  public:

    void set_ncryos(short ncryo);
    void set_apas(std::vector<short> napa, std::vector<double> angleU, std::vector<double> angleV, std::vector<double> pitch);
    void set_boundries(std::vector<std::vector<Vector> > center, std::vector<std::vector<Vector> > halves);
    void buildGDS();

    const WrappedGDS* get_apaGDS(short cryo, short apa) const {return _APAgds.at(cryo).at(apa);}
    
    short ncryos() const {return _ncryos;}
    short napa(short cryo) const {return _napas.at(cryo);}

    /*
    bool contained(const Vector& point) const;
    bool contained_yz(const Vector& point) const;
    short contained_cryo(const Vector& point) const;
    short contained_apa(short cryo, const Vector& point) const;
    short contained_yz_cryo(const Vector& point) const;
    short contained_yz_apa(short cryo, const Vector& point) const;
    */
    
  private:

    std::vector<std::vector<WrappedGDS*> > _APAgds;
    
    short _ncryos;
    std::vector<short> _napas;

    std::vector<std::vector<Vector> > _max_bound; // [cryo][apa]
    std::vector<std::vector<Vector> > _min_bound; // [cryo][apa]
    std::vector<std::vector<Vector> > _center;
    std::vector<std::vector<Vector> > _halves;
    std::vector<double> _angleU, _angleV, _pitch;
    
  };

}

#endif
