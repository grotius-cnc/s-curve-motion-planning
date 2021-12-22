# S-curve-motion-planning, 

[![grotius-cnc - s-curve-motion-planning](https://img.shields.io/static/v1?label=grotius-cnc&message=s-curve-motion-planning&color=blue&logo=github)](https://github.com/grotius-cnc/s-curve-motion-planning "Go to GitHub repo")
[![stars - s-curve-motion-planning](https://img.shields.io/github/stars/grotius-cnc/s-curve-motion-planning?style=social)](https://github.com/grotius-cnc/s-curve-motion-planning)
[![forks - s-curve-motion-planning](https://img.shields.io/github/forks/grotius-cnc/s-curve-motion-planning?style=social)](https://github.com/grotius-cnc/s-curve-motion-planning)

[![GitHub release](https://img.shields.io/github/release/grotius-cnc/s-curve-motion-planning?include_prereleases=&sort=semver&color=blue)](https://github.com/grotius-cnc/s-curve-motion-planning/releases/)
[![License](https://img.shields.io/badge/License-MIT-blue)](#license)

Using example functions:

    example_lineair();
    Output: dispalcement[st]:1.5 at_time[i]0.3
    
    example_scurve();
    Output: at_time:0.310 sr:0.218 vr:0.202 ar:-2.798 ct:0.455
    
Lineair stage request:

    int scurve_lineair(double at_time, double ve, double s, double &st, double &ct);
    
S-curve acc or dcc stage request:

   int Scurve::scurve_acc_dcc(  int sct, 
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

    motion_block(double vs, double am, double vo, double acs, double ltot, double ve, double ace, double at_time, double &sr,double &vr, double &ar ,double &ct);
    
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

## License

Released under [MIT](/LICENSE) by [@grotius-cnc](https://github.com/grotius-cnc).
