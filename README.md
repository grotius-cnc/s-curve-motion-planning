# S-curve-motion-planning.

[![grotius-cnc - s-curve-motion-planning](https://img.shields.io/static/v1?label=grotius-cnc&message=s-curve-motion-planning&color=blue&logo=github)](https://github.com/grotius-cnc/s-curve-motion-planning "Go to GitHub repo")
[![stars - s-curve-motion-planning](https://img.shields.io/github/stars/grotius-cnc/s-curve-motion-planning?style=social)](https://github.com/grotius-cnc/s-curve-motion-planning)
[![forks - s-curve-motion-planning](https://img.shields.io/github/forks/grotius-cnc/s-curve-motion-planning?style=social)](https://github.com/grotius-cnc/s-curve-motion-planning)

[![GitHub release](https://img.shields.io/github/release/grotius-cnc/s-curve-motion-planning?include_prereleases=&sort=semver&color=blue)](https://github.com/grotius-cnc/s-curve-motion-planning/releases/)
[![License](https://img.shields.io/badge/License-MIT-blue)](#license)

The purpose of this material is to impart an understanding of polynomial transitions for a trajectory generator. The ideal constant jerk S-curve
(jerk is the derivate of acceleration and a measure of impact) can be represented by a second-order polynomial in velocity (3rd in position). 
Its shape is governed by the motion conditions at the start and end of the transition.

An S-curve with an intermediate constant acceleration (lineair portion) is often used to reduce the time to make large speed changes. The jerk can be
used to determine how much of the rise or fall period can be made under constant acceleration.

![example](https://user-images.githubusercontent.com/44880102/147084953-56985394-e260-43b2-83bf-e4379026f9ed.jpg)

Using example functions:

    void example_lineair();
        Output: at_time:0.000 sr:2.000 vr:5.000 ar:0.000 ct:0.000
    
    void example_scurve();
        Output: at_time:4.960 sr:12.300 vr:4.999 ar:0.032 ct:5.000
    
    void example_motion();
        Output: at_time:14.800 sr:99.998 vr:0.032 ar:-0.320 ct:15.000
        Time taken by function nanoseconds: 1397 milliseconds:0.001
    
Lineair stage request:

                        //! Inputs:
                        //! at_time=request displacment at a certain time stamp.
                        //! ve=velocity
                        //! s=total displacement
                        //! Outputs:
                        //! st=displacment at_time request.
                        //! ct=total curve time.

       int scurve_lineair( double at_time,     
                        double vs, 
                        double s, 
                        double &ct, 
                        double &sr){
    
S-curve acc or dcc stage request:

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
                        //! ct=total curve time [t] excluding acc start time, acc end time.
                        //! cs=total curve displacement.

    int Scurve::scurve_acc_dcc( int sct, 
                            double vo, 
                            double ve, 
                            double am, 
                            double acs, 
                            double ace, 
                            double at_time, 
                            double &sr, 
                            double &vr, 
                            double &ar, 
                            double &ct, 
                            double &cs);

Motion block request:

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

      int motion_block( double vs, 
                        double am, 
                        double vo, 
                        double acs, 
                        double ltot, 
                        double ve, 
                        double ace, 
                        double at_time, 
                        double &sr,
                        double &vr, 
                        double &ar,
                        double &ct);

example_0.c request a example function

                        #include <stdio.h>
                        #include "scurve.h"

                        //! Install scurve library:
                        //! sudo make install

                        //! Compile :
                        //! g++ example_0.c -o example_0 -L$PWD -lscurve

                        //! Run:
                        //! ./main

                        int main(int argc, char *argv[]) {
                            Scurve().example_motion();
                        }

                        //! Output :
                        //! at_time:0 sr:0.0576 vr:0.288 ar:0.96 ct:15
                        //! Time taken by function nanoseconds: 7473 milliseconds:0.00747

example_1.c request a complex motion 

                        //! Install scurve library:
                        //! sudo make install

                        //! Compile :
                        //! g++ example_1.c -o example_1 -L$PWD -lscurve

                        //! Run:
                        //! ./example_1

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

                        int main(int argc, char *argv[]) {

                            double vs=10; 
                            double am=2;
                            double vo=0;
                            double acs=0;
                            double ltot=100;
                            double ve=0;
                            double ace=0;
                            double at_time=0; 
                            double sr=0;
                            double vr=0; 
                            double ar=0;
                            double ct=0;

                            int r=0;
                            //! Request total motion time [ct]
                            r+=Scurve().motion_block( 	vs, 
                                                        am, 
                                                        vo, 
                                                        acs, 
                                                        ltot, 
                                                        ve, 
                                                        ace, 
                                                        at_time, 
                                                        sr,
                                                        vr, 
                                                        ar,
                                                        ct);
                            if(!r){									
                                printf("traject time: %f \n", ct);
                            } else {printf("error \n");}

                            //! For the total motion time [ct], request a state at time stamp [t]
                            for(double t=0; t<ct; t+=0.1){
                                r+=Scurve().motion_block(	vs, 
                                                            am, 
                                                            vo, 
                                                            acs, 
                                                            ltot, 
                                                            ve, 
                                                            ace, 
                                                            t, 
                                                            sr,
                                                            vr, 
                                                            ar,
                                                            ct);
                                if(!r){							
                                    printf("current displacment : %f \n", sr);
                                    printf("current velocity    : %f \n", vr);
                                    printf("current acceleration: %f \n", ar);	
                                } else {printf("error \n");}
                            }													
                        }

Implementation:

- Velocity up s-curve [acc period].
- Velocity down s-curve [dcc period].
- Acceleration begin value.
- Acceleration end value.
- Velocity begin.
- Velocity end.
- Max acceleration.
- Lineair stage.
- Scientific papers included.

Build in Logic:

- If maximum velocity can not be reached for a motion, curves are sampled to fit.
- Debug information when input values are out of scope.

Performance:

    Time taken to calculate a motion nanoseconds: ~1500 ~4051 

Graphic implementation:

    ~/gui_project/motion/
    
![scurvemotion](https://user-images.githubusercontent.com/44880102/147080009-f6e50645-2be6-46e6-a253-6fbf8488c1de.jpg)

To use the opencascade graphics along with the gui project, follow these instructions : https://github.com/grotius-cnc/oce/releases/tag/1.0.1

## Summary
Constant jerk S-curves are used to transition robot moves. The S-curve is used to eliminate discontinuities in acceleration that are detrimental
to robot performance.

## License

Released under [MIT](/LICENSE) by [@grotius-cnc](https://github.com/grotius-cnc).
