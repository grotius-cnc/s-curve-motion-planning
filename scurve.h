#ifndef SCURVE_H
#define SCURVE_H

#include "iostream"
#include "chrono"

//! This shared library is written by Skynet Cyberdyne.
//! No green pass is required to use this software, just use your mind.

class Scurve
{
public:
    Scurve();

    struct RESULT{
        //! Displacement result.
        double sr=0;
        //! Velocity result.
        double vr=0;
        //! Acceleration result.
        double ar=0;
        //! Curve time.
        double ct=0;
        //! Curve displacment.
        double cs=0;
        //! Error.
        bool error=0;
    };

    //! Inputs:
    //! at_time=request displacment at a certain time stamp.
    //! vs=velocity max.
    //! cs=curve displacement
    RESULT scurve_lineair(double at_time, double vs, double cs);

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
    RESULT scurve_acc_dcc(int sct, double vo, double ve, double am, double acs, double ace, double at_time);

    //! Inputs:
    //! vs=velocity max.
    //! am=acceleration max.
    //! vo=velocity begin.
    //! acs=acceleration begin.
    //! ltot=pathlenght.
    //! ve=velocity end.
    //! ace=acceleration end.
    //! at_time=curve request at time [t]
    RESULT motion(double vs, double am, double vo, double acs, double ltot, double ve, double ace, double at_time);

    void example();
};


#endif // SCURVE_H




















