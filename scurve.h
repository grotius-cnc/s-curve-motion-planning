#ifndef SCURVE_H
#define SCURVE_H

#include "scurve_global.h"
#include "iostream"
#include "chrono"

//! This shared library is written by Skynet Cyberdyne.
//! No green pass is required to use this software, just use your mind.

class SCURVE_EXPORT Scurve
{
public:
    Scurve();

    //! Inputs:
    //! at_time=request displacment at a certain time stamp.
    //! ve=velocity
    //! s=total displacement
    //! Outputs:
    //! st=displacment at_time request.
    //! ct=total curve time.
    int scurve_lineair(double at_time, double ve, double s, double &st, double &ct);
    void example_lineair();

    //! Inputs:
    //! sct=scurve type [0=acc, 1=dcc]
    //! vo=velocity start
    //! vs=velocity max
    //! ve=velocity end
    //! am=acceleration max
    //! acs=acceleration start
    //! ace=acceleration end
    //! Results:
    //! at_time=request curve at [t]
    //! vr=current_velocity at [t]
    //! ar=current acceleration at [t]
    //!
    //! ct=netto curve time [t] excluding acc start time, acc end time.
    //! cs=netto curve displacement.
    //!
    int scurve_acc_dcc(int sct, double vo, double ve, double am, double acs, double ace, double at_time, double &sr, double &vr, double &ar, double &ct, double &cs);
    void example_scurve();

    //! Inputs:
    //! vs=velocity max.
    //! am=acceleration max.
    //! ltot=pathlenght.
    //! vo=velocity begin.
    //! acs=acceleration begin.
    //! ve=velocity end.
    //! ace=acceleration end.
    //! at_time=curve request at time [t]
    //! Results:
    //! sr=current displacement.
    //! vr=current velocity.
    //! ar=current acceleration.
    //! ct=total curve time.
    int motion_block(double vs, double am, double vo, double acs, double ltot, double ve, double ace, double at_time, double &sr, double &vr, double &ar , double &ct);
    void example_motion();
};

#endif // SCURVE_H
