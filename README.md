# S-curve-motion-planning, 

Provided as shared library, c & c++ compatible.

Using example functions:

    example_lineair();
    Output: dispalcement[st]:1.5 at_time[i]0.3
    
    example_scurve();
    Output: at_time:0.310 sr:0.218 vr:0.202 ar:-2.798 ct:0.455
    
Lineair stage request:

    int scurve_lineair(double at_time, double ve, double s, double &st, double &ct);
    
S-curve acc or dcc stage request:

    int scurve_acc_dcc(int sct, double vo, double ve, double am, double acs, double ace, double at_time, double &sr, double &vr, double &ar, double &ct);
    
        
Implementition:
- velocity up s-curve [acc period]
- velocity down s-curve [dcc period].
- acceleration begin value.
- acceleration end value.
- velocity begin.
- velocity end.
- max acceleration.
- lineair stage.
- scientific papers included.

~/gui_project/motion example output:

![scurve_acc_dcc](https://user-images.githubusercontent.com/44880102/146907278-0098c91a-85bc-44e6-95a0-26e20ad44f95.jpg)

To use the opencascade graphics with the gui project, follow these instructions : https://github.com/grotius-cnc/oce/releases/tag/1.0.1

