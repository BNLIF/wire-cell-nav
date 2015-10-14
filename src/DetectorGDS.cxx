#include "WireCellNav/DetectorGDS.h"

#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;
using namespace WireCell;

WireCell::DetectorGDS::DetectorGDS()
  : _ncryos(0)
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

WireCell::DetectorGDS::DetectorGDS(std::vector<std::string> geometry)
{
    _ncryos = geometry.size();
    _APAgds.resize(_ncryos);
    _napas.resize(_ncryos);
    _angleU.resize(_ncryos);
    _angleV.resize(_ncryos);
    _pitch.resize(_ncryos);
    _max_bound.resize(_ncryos);
    _min_bound.resize(_ncryos);
    _center.resize(_ncryos);
    _halves.resize(_ncryos);
    
    for ( short cryo = 0; cryo < _ncryos; cryo++ ) {
        ifstream infile(geometry.at(cryo).c_str());
	std::string tmp;
	infile >> tmp >> _pitch.at(cryo);
	//std::cout<<tmp.c_str()<<" is "<<_pitch.at(cryo)<<"\n";
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
	    std::cout<<"bound box: ("<<_min_bound.at(cryo).at(apa).x<<", "<<_min_bound.at(cryo).at(apa).y<<", "<<_min_bound.at(cryo).at(apa).z
	    	     <<") --> ("<<_max_bound.at(cryo).at(apa).x<<", "<<_max_bound.at(cryo).at(apa).y<<", "<<_max_bound.at(cryo).at(apa).z<<")"<<std::endl;
	    _APAgds.at(cryo).at(apa) = new WrappedGDS(_min_bound.at(cryo).at(apa), _max_bound.at(cryo).at(apa), _angleU.at(cryo), _angleV.at(cryo), _pitch.at(cryo), cryo, apa);
	}
	
    }
    
}

void DetectorGDS::set_ncryos(short ncryos)
{
  _ncryos = ncryos;
}

void DetectorGDS::set_apas(std::vector<short> napa, std::vector<double> angleU, std::vector<double> angleV, std::vector<double> pitch)
{
  _napas = napa;
  _angleU = angleU;
  _angleV = angleV;
  _pitch = pitch;
}

void DetectorGDS::set_boundries(std::vector<std::vector<Vector> > center, std::vector<std::vector<Vector> > halves)
{
  _center = center;
  _halves = halves;
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
	    _APAgds.at(cryo).at(apa) = new WrappedGDS(_min_bound.at(cryo).at(apa), _max_bound.at(cryo).at(apa), _angleU.at(cryo), _angleV.at(cryo), _pitch.at(cryo), cryo, apa);
	}
	
    }
}

short DetectorGDS::in_which_apa(const Vector& point) const
{
  short a = -999;
  for (short cryo = 0; cryo < _ncryos; cryo++) {
    for (short apa = 0; apa < _napas.at(cryo); apa++) {
      if (_APAgds.at(cryo).at(apa)->contained_yz(point)) {
	a = apa;
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
      }
    }
  }
  return c;
}

WrappedGDS* DetectorGDS::get_apaGDS(short cryo, short apa) const
{
  WrappedGDS* gds = NULL;
  if (cryo >= 0 && apa >=0) {
    gds = _APAgds.at(cryo).at(apa);
  }
  return gds;
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
