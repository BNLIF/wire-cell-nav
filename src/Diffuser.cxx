#include "WireCellNav/Diffuser.h"

#include <cmath>

#include <iostream>
using namespace std;		// debugging

using namespace WireCell;

Diffuser::Diffuser(BufferedHistogram2D& hist, int nsigma)
//		   double drift_velocity, double dl, double dt)
    : m_hist(hist)
    , m_nsigma(nsigma)
//  , m_drift_velocity(drift_velocity)
//  , m_DL(dl)
//  , m_DT(dt)
{
}
Diffuser::~Diffuser()
{
}

std::vector<double> Diffuser::oned(double mean, double sigma, double origin, double binsize) const
{
    // find the high/low edges of bins containing nsigma 
    int bin_edge_low = floor((mean - m_nsigma*sigma - origin)/binsize);
    int bin_edge_high = ceil((mean + m_nsigma*sigma - origin)/binsize);
    if (bin_edge_high == bin_edge_low) {
	++bin_edge_high;
	--bin_edge_low;
    }
    if (bin_edge_low < 0) { bin_edge_low = 0; }
    int nbins = bin_edge_high - bin_edge_low; // >= 1

    cerr << mean << " +/- " << sigma
    	 << " " << origin << " by " << binsize << " [ " << bin_edge_low << " , " << bin_edge_high << " ] " << endl;

    std::vector<double> integral(bin_edge_high+1, 0.0);
    for (int ibin=bin_edge_low; ibin <= bin_edge_high; ++ibin) {
	double absx = origin + ibin*binsize;
	double t = 0.5*(absx-mean)/sigma;
	double e = 0.5*std::erf(t);
	cerr << "erf: " << ibin << " t=" << t << " erf=" << e << endl;
	integral[ibin] = e;
    }

    std::vector<double> bins(bin_edge_high,0.0);
    for (int ibin=bin_edge_low; ibin < bin_edge_high; ++ibin) {
	bins[ibin] = integral[ibin+1] - integral[ibin];
	cerr << "prob: " << ibin << ": erf["<<ibin+1<<"]=" << integral[ibin+1] << " - erf["<<ibin<<"]=" << integral[ibin]
	     << " = " << bins[ibin] << endl;
    }
    return bins;
}

double Diffuser::diffuse(double x, double y, double sigma_x, double sigma_y, double q)
{
    std::vector<double> xbins = oned(x, sigma_x, m_hist.xmin(), m_hist.xbinsize());
    std::vector<double> ybins = oned(y, sigma_y, m_hist.ymin(), m_hist.ybinsize());

    cerr << "Bins: " << xbins.size() << " X " << ybins.size() << endl;

    // get normalization
    double power = 0;
    for (int xind = 0; xind < xbins.size(); ++xind) {
	for (int yind = 0; yind < ybins.size(); ++yind) {
	    double p = xbins[xind]*ybins[yind];
	    //cerr << xind << "," << yind << ": " << xbins[xind] << "," << ybins[yind] << " = " << p << endl;
	    power += p;
	}
    }
    cerr << "\tpower = " << power << endl;
    
    for (int xind = 0; xind < xbins.size(); ++xind) {
	for (int yind = 0; yind < ybins.size(); ++yind) {
	    double xval = m_hist.xmin() + (xind+0.5)*m_hist.xbinsize();
	    double yval = m_hist.ymin() + (yind+0.5)*m_hist.ybinsize();
	    double val = xbins[xind]*ybins[yind]*q/power;
	    if (val <= 0.0) {
		continue;
	    }
	    bool ok = m_hist.fill(xval, yval, val);
	    if (!ok) {
		cerr << "underflow: " << "(" << xval << "," << yval << ") = " << val << endl;
	    }
	}
    }
    

    // return lowest new data
    for (int xind = 0; xind < xbins.size(); ++xind) {
	if (xbins[xind] > 0.0) {
	    return m_hist.xmin() + xind * m_hist.xbinsize();
	}
    }
    return m_hist.xmin();
}
