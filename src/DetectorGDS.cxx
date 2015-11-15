#include "WireCellNav/DetectorGDS.h"

#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;
using namespace WireCell;

WireCell::DetectorGDS::DetectorGDS()
//  : _ncryos(0)
{
}

WireCell::DetectorGDS::~DetectorGDS()
{
    for (short cryo = 0; cryo < _ncryos; cryo++) {
        for (short apa = 0; apa < _napas.at(cryo); apa++) {
	    if (_APAgds.at(cryo).at(apa) != NULL) {
	        delete _APAgds.at(cryo).at(apa);
	    }
	}
    }
}

int WireCell::DetectorGDS::get_total_nwires(WirePlaneType_t plane) const{
  int nwire = 0;
  for (short cryo = 0; cryo < _ncryos; cryo++) {
        for (short apa = 0; apa < _napas.at(cryo); apa++) {
	  nwire +=  _APAgds.at(cryo).at(apa)->wires_in_plane(plane).size();
	}
  }
  return nwire;
}

const WireCell::GeomWireSelection& WireCell::DetectorGDS::by_channel(int channel) const{
  //const WireCell::GeomWireSelection& wires;
  for (short cryo = 0; cryo < _ncryos; cryo++) {
    for (short apa = 0; apa < _napas.at(cryo); apa++) {
      const WireCell::GeomWireSelection& wires = _APAgds.at(cryo).at(apa)->by_channel(channel);
      if (wires.size() >0) return wires;
      //std::cout << cryo << " " << apa << " " << wires.size() << std::endl;
    }
  }
  //return wires;
}

WireCell::DetectorGDS::DetectorGDS(std::vector<std::string> geometry)
{
    _ncryos = geometry.size();
    _APAgds.resize(_ncryos);
    _napas.resize(_ncryos);
    _angleU.resize(_ncryos);
    _angleV.resize(_ncryos);
    _pitchU.resize(_ncryos);
    _pitchV.resize(_ncryos);
    _pitchW.resize(_ncryos);
    _max_bound.resize(_ncryos);
    _min_bound.resize(_ncryos);
    _center.resize(_ncryos);
    _halves.resize(_ncryos);
    
    for ( short cryo = 0; cryo < _ncryos; cryo++ ) {
        ifstream infile(geometry.at(cryo).c_str());
	std::string tmp;
	infile >> tmp >> _pitchU.at(cryo);
	infile >> tmp >> _pitchV.at(cryo);
	infile >> tmp >> _pitchW.at(cryo);
	infile >> tmp >> _angleU.at(cryo);
	//std::cout<<tmp.c_str()<<" is "<<_angleU.at(cryo)<<"\n";
	infile >> tmp >> _angleV.at(cryo);
	//std::cout<<tmp.c_str()<<" is "<<_angleV.at(cryo)<<"\n";
	infile >> tmp >> _napas.at(cryo);
	//std::cout<<"number of APAs is "<<_napas.at(cryo)<<std::endl;
	_max_bound.at(cryo).resize(_napas.at(cryo));
	_min_bound.at(cryo).resize(_napas.at(cryo));
	_center.at(cryo).resize(_napas.at(cryo));
	_halves.at(cryo).resize(_napas.at(cryo));
	_APAgds.at(cryo).resize(_napas.at(cryo));
	for (short apa = 0; apa < _napas.at(cryo); apa++) {
	    infile >> tmp >> _center.at(cryo).at(apa).x >> _center.at(cryo).at(apa).y >> _center.at(cryo).at(apa).z;
	    infile >> tmp >> _halves.at(cryo).at(apa).x >> _halves.at(cryo).at(apa).y >> _halves.at(cryo).at(apa).z;	 
	    _min_bound.at(cryo).at(apa) = _center.at(cryo).at(apa) - _halves.at(cryo).at(apa);
	    _max_bound.at(cryo).at(apa) = _center.at(cryo).at(apa) + _halves.at(cryo).at(apa);
	    //std::cout<<"bound box: ("<<_min_bound.at(cryo).at(apa).x<<", "<<_min_bound.at(cryo).at(apa).y<<", "<<_min_bound.at(cryo).at(apa).z
	    //	     <<") --> ("<<_max_bound.at(cryo).at(apa).x<<", "<<_max_bound.at(cryo).at(apa).y<<", "<<_max_bound.at(cryo).at(apa).z<<")"<<std::endl;
	    _APAgds.at(cryo).at(apa) = new WrappedGDS(_min_bound.at(cryo).at(apa), _max_bound.at(cryo).at(apa), _angleU.at(cryo), _angleV.at(cryo), _pitchU.at(cryo), _pitchV.at(cryo), _pitchW.at(cryo), cryo, apa);
	}
	
    }
    
}

void DetectorGDS::set_ncryos(short ncryos)
{
  _ncryos = ncryos;
  _napas.resize(_ncryos);
  _angleU.resize(_ncryos);
  _angleV.resize(_ncryos);
  _pitchU.resize(_ncryos);
  _pitchV.resize(_ncryos);
  _pitchW.resize(_ncryos);
  _center.resize(_ncryos);
  _halves.resize(_ncryos);
}

void DetectorGDS::set_napas(short cryo, short napas)
{
  _napas.at(cryo) = napas;
  _center.at(cryo).resize(napas);
  _halves.at(cryo).resize(napas);
}

void DetectorGDS::set_apas(std::vector<short> napa, std::vector<double> angleU, std::vector<double> angleV, std::vector<double> pitchU, std::vector<double> pitchV, std::vector<double> pitchW, std::vector<std::vector<Vector> > center, std::vector<std::vector<Vector> > halves)
{
  _napas = napa;
  _angleU = angleU;
  _angleV = angleV;
  _pitchU = pitchU;
  _pitchV = pitchV;
  _pitchW = pitchW;
  _center = center;
  _halves = halves;
}

void DetectorGDS::set_apa(short cryo, short apa, double angleU, double angleV, double pitchU, double pitchV, double pitchW, Vector center, Vector halves)
{
  _angleU.at(cryo) = angleU;
  _angleV.at(cryo) = angleV;
  _pitchU.at(cryo) = pitchU;
  _pitchV.at(cryo) = pitchV;
  _pitchW.at(cryo) = pitchW;
  _center.at(cryo).at(apa) = center;
  _halves.at(cryo).at(apa) = halves;
}

void DetectorGDS::buildGDS()
{
    _APAgds.resize(_ncryos);
    _max_bound.resize(_ncryos);
    _min_bound.resize(_ncryos);
    
    for ( short cryo = 0; cryo < _ncryos; cryo++ ) {
	_max_bound.at(cryo).resize(_napas.at(cryo));
	_min_bound.at(cryo).resize(_napas.at(cryo));
	_APAgds.at(cryo).resize(_napas.at(cryo));
	for (short apa = 0; apa < _napas.at(cryo); apa++) {
	    _min_bound.at(cryo).at(apa) = _center.at(cryo).at(apa) - _halves.at(cryo).at(apa);
	    _max_bound.at(cryo).at(apa) = _center.at(cryo).at(apa) + _halves.at(cryo).at(apa);
	    _APAgds.at(cryo).at(apa) = new WrappedGDS(_min_bound.at(cryo).at(apa), _max_bound.at(cryo).at(apa), _angleU.at(cryo), _angleV.at(cryo), _pitchU.at(cryo), _pitchV.at(cryo), _pitchW.at(cryo), cryo, apa);
	}	
    }

    build_channel_map();
}

void DetectorGDS::build_channel_map(){
   
  int nu_wire = 0;
  int nv_wire = 0;
  int nw_wire = 0;
  
  for (short cryo = 0; cryo < _ncryos; cryo++) {
    for (short apa = 0; apa < _napas.at(cryo); apa++) {
      // deal with u-plane
      const GeomWireSelection& uwires = _APAgds.at(cryo).at(apa)->wires_in_plane(WirePlaneType_t(0));
      for (int i = 0; i!=uwires.size();i++){
	int chid = uwires.at(i)->channel();
	if (_channel_umap.find(chid) == _channel_umap.end()){
	  _channel_umap[chid] = nu_wire;
	  nu_wire++;
	}
      }

      //deal with v-map
      const GeomWireSelection& vwires = _APAgds.at(cryo).at(apa)->wires_in_plane(WirePlaneType_t(1));
      for (int i = 0; i!=vwires.size();i++){
	int chid = vwires.at(i)->channel();
	if (_channel_vmap.find(chid) == _channel_vmap.end()){
	  _channel_vmap[chid] = nv_wire;
	  nv_wire++;
	}
      }
      
      //deal with w-map
      const GeomWireSelection& wwires = _APAgds.at(cryo).at(apa)->wires_in_plane(WirePlaneType_t(2));
      for (int i = 0; i!=wwires.size();i++){
	int chid = wwires.at(i)->channel();
	if (_channel_wmap.find(chid) == _channel_wmap.end()){
	  _channel_wmap[chid] = nw_wire;
	  nw_wire++;
	}
      }

      

    }
  }
  
}

int DetectorGDS::channel_count_conv(int channel) const{
  if (_channel_umap.find(channel)!=_channel_umap.end()){
    return _channel_umap.at(channel);
   
  }else if (_channel_vmap.find(channel)!=_channel_vmap.end()){
    return _channel_vmap.at(channel);
  }else if (_channel_wmap.find(channel)!=_channel_wmap.end()){
    return  _channel_wmap.at(channel);
  }else{
    return -1;
  }
}

WirePlaneType_t DetectorGDS::channel_plane_conv(int channel) const{
  if (_channel_umap.find(channel)!=_channel_umap.end()){
    return WirePlaneType_t(0);
  }else if (_channel_vmap.find(channel)!=_channel_vmap.end()){
    return WirePlaneType_t(1);
  }else if (_channel_wmap.find(channel)!=_channel_wmap.end()){
    return WirePlaneType_t(2);
  }else{
    return kUnknownWirePlaneType;
  }
}

int DetectorGDS::channel_count(WirePlaneType_t plane) const{
  if (plane == WirePlaneType_t(0)){
    return _channel_umap.size();
  }else if (plane == WirePlaneType_t(1)){
    return _channel_vmap.size();
  }else if (plane == WirePlaneType_t(2)){
    return _channel_wmap.size();
  }
}

short DetectorGDS::in_which_apa(const Vector& point) const
{
  short a = -999;
  for (short cryo = 0; cryo < _ncryos; cryo++) {
    for (short apa = 0; apa < _napas.at(cryo); apa++) {
      if (_APAgds.at(cryo).at(apa)->contained_yz(point)) {
	a = apa;
	return a;
      }
    }
  }
  return a;
}

short DetectorGDS::in_which_cryo(const Vector& point) const
{
  short c = -999;
  for (short cryo = 0; cryo < _ncryos; cryo++) {
    for (short apa = 0; apa < _napas.at(cryo); apa++) {
      if (_APAgds.at(cryo).at(apa)->contained_yz(point)) {
	c = cryo;
	return c;
      }
    }
  }
  return c;
}

const WrappedGDS* DetectorGDS::get_apaGDS(short cryo, short apa) const
{
  const WrappedGDS* gds = NULL;
  if (cryo >= 0 && apa >=0 && cryo < _ncryos && apa < _napas.at(cryo)) {
    gds = _APAgds.at(cryo).at(apa);
  }
  return gds;
}

void DetectorGDS::set_apaGDS(short cryo, short apa, const WrappedGDS *apaGDS)
{
  if (cryo >=0 && apa >=0 && cryo < _ncryos && apa < _napas.at(cryo)) {
    _APAgds.at(cryo).at(apa) = apaGDS;
  }
}

double DetectorGDS::get_angle(short cryo, WirePlaneType_t plane) const
{
    switch (plane) {
    case 0:
        return _angleU.at(cryo);
    case 1:
        return _angleV.at(cryo);
    case 2:
        return 0.;
    default:
        return -999.;
    }
}

double DetectorGDS::get_pitch(short cryo, WirePlaneType_t plane) const
{
  switch (plane) {
    case 0:
        return _pitchU.at(cryo);
    case 1:
        return _pitchV.at(cryo);
    case 2:
        return _pitchW.at(cryo);
    default:
        return -999.;
    }
}

bool DetectorGDS::crossing_point(double dis1, double dis2, const GeomWire& wire1, const GeomWire& wire2, Vector& result) const
{
  if ((wire1.apa() != wire2.apa()) || (wire1.cryo() != wire2.cryo()) || (wire1.face() != wire2.face()) || (wire1.plane() == wire2.plane())) {
    return false;
  }
  const WrappedGDS *apaGDS = get_apaGDS(wire1.cryo(), wire1.apa());
  Vector minbound = apaGDS->get_minbound();
  Vector maxbound = apaGDS->get_maxbound();
  double theta1 = get_angle(wire1.cryo(),wire1.plane());
  double theta2 = get_angle(wire2.cryo(),wire2.plane());

  if (wire1.face()) {
    theta1 *= -1;
    theta2 *= -1;
  }
  
  double a1 = std::cos(theta1);
  double b1 = -std::sin(theta1);
  
  double a2 = std::cos(theta2);
  double b2 = -std::sin(theta2);

  //equation array is
  // dis1 = z * a1 + y * b1;
  // dis2 = z * a2 + y * b2;

  result.y = (dis1 * a2 - dis2 * a1)/(b1 * a2 - b2 * a1);
  result.z = (dis1 * b2 - dis2 * b1)/(a1 * b2 - a2 * b1);

  if (result.y < minbound.y || maxbound.y < result.y) { 
    return false;
  }

  if (result.z < minbound.z || maxbound.z < result.z) { 
    return false;
  }

  return true;
    
}

const GeomWire* DetectorGDS::closest(const Vector& point, WirePlaneType_t plane, int face) const
{
  const WrappedGDS* apaGDS = get_apaGDS(in_which_cryo(point),in_which_apa(point));
  const GeomWire* wire;
  if (apaGDS != NULL) {
    wire = apaGDS->closest(point, plane, face);
  }
  return wire;
}
