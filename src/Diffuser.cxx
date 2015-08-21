#include "WireCellNav/Diffuser.h"

using namespace WireCell;

Diffuser::Diffuser(BufferedHistogram2D& hist, Ray pitch, double drift_velocity,
		   double dl, double dt)
    : m_hist(hist)
    , m_pitch(pitch)
    , m_pitch_unit(ray_unit(pitch))
    , m_drift_velocity(drift_velocity)
    , m_pitch_dist(ray_length(pitch))
    , m_DL(dl)
    , m_DT(dt)
{
}
Diffuser::~Diffuser()
{
}

double Diffuser::diffuse(IDepo::pointer depo)
{
    // Drift the depo the last little distance to my plane.
    // Note, this assumes drifting only in -X direction, no wiggles!
    IDepo::pointer last_depo = *depo_chain(depo).rbegin();
    const double start_x = last_depo->pos().x();
    const double my_x = m_pitch.first.x();
    const double drift_dist = start_x - my_x;
    const double drift_time = drift_dist / m_drift_velocity;
    Vector depo_pos = depo->pos();

    // find longitudinal where/when
    const double depo_time = depo->time() + (depo_pos.x() - my_x)/m_drift_velocity;
    depo_pos.x(my_x);

    // find transverse where 
    const Vector todepo = depo_pos - m_pitch.first; // vector from wire zero center to deposition.
    const double dist = todepo.dot(m_pitch_unit); // distance along pitch from wire zero.
    const double pdist = dist/m_pitch_dist; // distance in units of pitch.
    const int wind = int(pdist+0.5); // wire index of closest wire

    const double sigmaL = sqrt(2*m_DL*drift_time) / m_drift_velocity;
    const double sigmaT = sqrt(2*m_DT*drift_time) / m_drift_velocity;

    ////...............................
}
