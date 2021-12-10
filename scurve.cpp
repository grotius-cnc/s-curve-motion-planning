#include "scurve.h"

scurve::scurve()
{

}

void scurve::demo_traject(){

    //! Request position at 1 sec.
    double at_second=1;

    struct result r;                                    //! Structure that holds the results.
    r=request_trajectcalculator_scurve(5,			 	//! velmax.
                                       2,               //! accmax.
                                       3,               //! dccmax.
                                       0,               //! accbegin.
                                       0,               //! dccend.
                                       1, 				//! vbegin.
                                       0, 				//! vend.
                                       50,              //! pathlenght.
                                       at_second,       //! request at time(t) in sec.
                                       1);              //! debug 0 or 1.

    std::cout<<"displacment  :"<< r.s <<std::endl;
    std::cout<<"traject time :"<< r.t <<std::endl;
    std::cout<<"velocity     :"<< r.v <<std::endl;
    std::cout<<"acceleration :"<< r.a <<std::endl;
    std::cout<<"jerk         :"<< r.j <<std::endl;

    if(at_second>=r.t){ // Scurve completed, reset time.
        std::cout<<"scurve was completed"<<std::endl;
    }

    /* output of demo traject:
                    D ACCELERATION STAGE
                    Check 0, using (s_concave=(tr.Vo+(tr.As*tr.As)/(6*jm))*tr.As/jm) + (s_convex=(vh+(tr.As*tr.As)/(3*jm))*tr.As/jm)
                    displacement(s) acc period concave (curve up): 1.666667
                    displacement(s) acc period convex (curve down): 4.333333
                    displacement(s) total: 6.000000
                    Check 1, using (s_concave=tr.Vo*t+jm*(t*t*t)/6) + (s_convex=(vh*t)+(tr.As*(t*t)/2)-(jm*(t*t*t)/6))
                    displacement(s) total: 6.000000
                    Check 2, using ((tr.Vs*tr.Vs)-(tr.Vo*tr.Vo))/tr.As
                    displacement(s) total: 6.000000

                    TRAJECT CHECKS PERIOD DECCELERATION STAGE
                    displacement(s) dcc period convex (curve down): 3.472222
                    Check 0, using (s_concave=(tr.Ve+(tr.Ds*tr.Ds)/(6*jm))*tr.Ds/jm) + (s_convex=(vh+(tr.Ds*tr.Ds)/(3*jm))*tr.Ds/jm)
                    displacement(s) dcc period concave (curve up): 0.694444
                    displacement(s) total: 4.166667
                    Check 1, using (s_concave=tr.Ve*th+jm*(th*th*th)/6) + (s_convex=(vh*th)+(tr.Ds*(th*th)/2)-(jm*(th*th*th)/6))
                    displacement(s) total: 3.472222
                    Check 2, using ((tr.Vs*tr.Vs)-(tr.Vo*tr.Vo))/tr.As
                    displacement(s) dcc total: 4.166667

                    Total displacement scurve: 50.000000
                    time         :11.6333
                    displacment  :1.66667
                    velocity     :2
                    acceleration :4
                    jerk         :0
*/
}

//! Main scurve function. returns time[t], displacement[s], velocity[v], acceleration[a], jerk[j]
scurve::result scurve::request_trajectcalculator_scurve(
        double velmax,      //! Max velocity
        double accmax,      //! Max acceleration
        double dccmax,      //! Max decceleration
        double accbegin,    //! Acceleration begin, used in concave period only.
        double dccend,      //! Acceleartion end, used in concave period only.
        double vbegin,      //! Start velocity, initial velocity.
        double vend,        //! End velocity
        double pathlenght,
        double timestamp,
        int debug){

    //! Input solver with warning message.
    if(velmax==0){
        std::cout<<"input error velmax=0 set to 1"<<std::endl;
        velmax=1;
    }
    if(accmax==0){
        std::cout<<"input error accmax=0 set to 1"<<std::endl;
        accmax=1;
    }
    if(dccmax==0){
        std::cout<<"input error dccmax=0 set to 1"<<std::endl;
        dccmax=1;
    }
    if(pathlenght==0){
        std::cout<<"input error pathlenght=0 set to 1"<<std::endl;
        pathlenght=1;
    }

    struct traject tr;
    // Data.
    tr.Vs=velmax; // When velocity max can not be reached, Vs is sampled to the max velocity of path.
    tr.Vo=vbegin;
    tr.Ve=vend;
    tr.Acc_lineair=accmax;
    tr.Dcc_lineair=dccmax;
    tr.Acc_begin=accbegin;
    tr.Dcc_end=dccend;
    tr.Ltot=pathlenght;

    // Define limits to initial velocity
    if(tr.Vo>tr.Vs){
        tr.Vo=tr.Vs;
    }
    if(tr.Vo<0){
        tr.Vo=0;
    }

    // Define limits to end velocity
    if(tr.Ve>tr.Vs){
        tr.Ve=tr.Vs;
    }
    if(tr.Ve<0){
        tr.Ve=0;
    }

    // Formula.
    tr.As=tr.Acc_lineair*2;                     // Acceleration at s-curve inflation point = normal acceleration*2.
    tr.Ds=tr.Dcc_lineair*2;                     // Deacceleration at inflectionpoint = normal deacceleration*2

    // Acc path.
    tr.L1=((tr.Vs*tr.Vs)-(tr.Vo*tr.Vo))/tr.As;  // Document page 4 (5.16)
    tr.T1=2*(tr.Vs-tr.Vo)/tr.As;                // Document page 3, observations : T=2*delta v/As

    // Dcc path.
    tr.L3=((tr.Vs*tr.Vs)-(tr.Ve*tr.Ve))/tr.Ds;
    tr.T3=2*(tr.Vs-tr.Ve)/tr.Ds;

    // Steady path.
    tr.L2=pathlenght-tr.L1-tr.L3;
    tr.T2=tr.L2/tr.Vs;

    // Total.
    tr.Ttot=tr.T1+tr.T2+tr.T3;

    // Motion can not reach full speed, find acc dcc intersection.
    if(tr.L2<0){
        // Decrease Velocity until L2 is positive, if Acceleration stays the same value.
        double TempVel=tr.Vs;
        for(double i=TempVel; i>0; i-=0.01){

            tr.Vs=i;
            //std::cout<<"Vel:"<<tr.Vel<<std::endl;

            // Acc path.
            tr.L1=((tr.Vs*tr.Vs)-(tr.Vo*tr.Vo))/tr.As;

            // Dcc path.
            tr.L3=((tr.Vs*tr.Vs)-(tr.Ve*tr.Ve))/tr.Ds;

            // At speed path.
            tr.L2=tr.Ltot-tr.L1-tr.L3;

            if(tr.L2>0.00001){
                // Calculate new values.
                tr.T1=2*(tr.Vs-tr.Vo)/tr.As;
                tr.T3=2*(tr.Vs-tr.Ve)/tr.Ds;
                tr.T2=tr.L2/tr.Vs;
                tr.Ttot=tr.T1+tr.T2+tr.T3;

                break;
            }
        }
    }

    // Calculate displacments for tr.s_acc_concave etc.
    tr=acc_period(tr, debug);
    tr=steady_period(tr, debug);
    tr=dcc_period(tr, debug);

    struct result r;
    r=request(tr,timestamp);
    return r;
}

struct scurve::traject scurve::acc_period(struct traject tr, int debug){

    // *************** Acc period, curve up. ***************
    double jm=2*tr.As/tr.T1;                    // Document page 2.
    if(tr.Acc_begin>=(2*tr.Acc_lineair)){       // tr.Acc_begin may be max 2*Acc_lineair at inflection point.
        tr.Acc_begin=(2*tr.Acc_lineair)-0.1;    // added -0.1 to stay away from the inflection point.
    }

    tr.Acc_start_time=tr.Acc_begin/jm;
    double time_to_remove=tr.T1-tr.Acc_start_time;
    /*
     * Period concave, document page 1.
    for(double t=Acc_start_time; t<=tr.T1/2; t+=tr.timeinterval){

        double s=tr.Vo*t+jm*(t*t*t)/6;  // s=displacement.
        double v=tr.Vo+jm*(t*t)/2;      // v=velocity.
        double a=jm*t;                  // a=acceleration.

        if(debug){
            rtapi_print_msg(RTAPI_MSG_ERR,"s: %f \n", s);
            rtapi_print_msg(RTAPI_MSG_ERR,"v: %f \n", v);
            rtapi_print_msg(RTAPI_MSG_ERR,"a: %f \n", a);
            rtapi_print_msg(RTAPI_MSG_ERR,"jm: %f \n", jm);
        }
        // xgraph=(t);
    }
    */

    // Check displacement(s) concave curve, at 1/2 acc period. Document page 3
    tr.s_acc_concave =((tr.Vo+(tr.As*tr.As)/(6*jm))*tr.As)/jm;

    // Calculate the convec acc period, curve down. Document 5.3
    double t=tr.T1/2;
    double vh=tr.Vo+jm*(t*t)/2; // Velocity at start of convex period. Velocity at first inflection point.

    // *************** Acc period, curve down. ***************

    /*
     *
    for(double t=0; t<=tr.T1/2; t+=tr.timeinterval){

        double s=vh*t + tr.As*(t*t)/2 - jm*(t*t*t)/6;
        double v=vh + tr.As*t - jm*(t*t)/2;
        double a=tr.As-jm*t;
        double th=tr.T1/2;

        if(debug){
            rtapi_print_msg(RTAPI_MSG_ERR,"s: %f \n", s+tr.s_acc_concave);
            rtapi_print_msg(RTAPI_MSG_ERR,"v: %f \n", v);
            rtapi_print_msg(RTAPI_MSG_ERR,"a: %f \n", a);
            rtapi_print_msg(RTAPI_MSG_ERR,"jm: %f \n", jm*-1);
        }
        // xgraph=(t+th);
    }
    */

    if(tr.T1>0){
        if(debug){
            std::cout<<"TRAJECT CHECKS PERIOD ACCELERATION STAGE"<<std::endl;;
            std::cout<<"Check 0, using (s_concave=(tr.Vo+(tr.As*tr.As)/(6*jm))*tr.As/jm) + (s_convex=(vh+(tr.As*tr.As)/(3*jm))*tr.As/jm)"<<std::endl;
        }
        // Document page 3 (5.3)
        double s_concave= (tr.Vo + (tr.As*tr.As) / (6*jm) ) * tr.As / jm;
        if(debug){
            printf("displacement(s) acc period concave (curve up): %f \n",s_concave);
        }

        // Document page 4 (5.15)
        double s_convex= (vh+(tr.As*tr.As) / (3*jm) ) * tr.As / jm;
        if(debug){
            printf("displacement(s) acc period convex (curve down): %f \n",s_convex);
            printf("displacement(s) total: %f \n",s_convex+s_concave);
            printf("Check 1, using (s_concave=tr.Vo*t+jm*(t*t*t)/6) + (s_convex=(vh*t)+(tr.As*(t*t)/2)-(jm*(t*t*t)/6)) \n");
        }
        // Document page 2 (5.7)
        tr.s_acc_concave=tr.Vo*t+jm*(t*t*t)/6;
        // Document page 4 (5.11)
        tr.s_acc_convex= (vh*t) + (tr.As*(t*t)/2) - (jm*(t*t*t)/6);
        if(debug){
            printf("displacement(s) total: %f \n",tr.s_acc_concave+tr.s_acc_convex);
            printf("Check 2, using ((tr.Vs*tr.Vs)-(tr.Vo*tr.Vo))/tr.As \n");
        }
        // Document page 4 (5.16)
        tr.s_acc=((tr.Vs*tr.Vs)-(tr.Vo*tr.Vo))/tr.As;
        if(debug){
            printf("displacement(s) total: %f \n",tr.s_acc);
            printf(" \n");
        }

        // If the acceleration begin value is used, fot the pathlenght we can do:
        if(tr.Acc_begin>0){
            if(debug){
                printf("Check 3, using acceleration begin \n");
            }
            double ts=tr.Acc_start_time;
            double lenght=tr.Vo*ts+jm*(ts*ts*ts)/6; // Part of a concave period that is not used.

            tr.s_acc_concave-=lenght;   // Remove the not used part of the concave period.
            tr.s_acc_convex= (vh*t) + (tr.As*(t*t)/2) - (jm*(t*t*t)/6); // Full convex period

            tr.L1=tr.s_acc_concave+tr.s_acc_convex; // Calculate new traject lenghts and times.
            tr.Ltot=tr.L1+tr.L2+tr.L3;

            tr.T1=tr.T1-time_to_remove;
            tr.Ttot=tr.T1+tr.T2+tr.T3;

            // Calculate nev Velocity at start of curve:
            double Vstart=tr.Vo + tr.As*ts - jm*(ts*ts)/2;
            if(debug){
                printf("displacement(s) total: %f \n",tr.s_acc_concave+ tr.s_acc_convex);
                printf("velocity at start of curve: %f \n",Vstart);
                printf(" \n");
            }
        }
    }
    return tr;
}

struct scurve::traject scurve::steady_period(struct traject tr, int debug){
    // *************** Steady period, curve horizontal. ***************

    /*
    for(double t=0; t<=tr.T2; t+=tr.timeinterval){

        double v=tr.Vs;
        tr.s_steady=v*t;
        double a=0;
        double jerk=0;

        if(debug){
            rtapi_print_msg(RTAPI_MSG_ERR,"s: %f \n", tr.s_acc+tr.s_steady);
            rtapi_print_msg(RTAPI_MSG_ERR,"v: %f \n", v);
            rtapi_print_msg(RTAPI_MSG_ERR,"a: %f \n", a);
            rtapi_print_msg(RTAPI_MSG_ERR,"jm: %f \n", jerk);
        }
        // xgraph=(t+tr.T1);
    }
    */

    // Complete value:
    tr.s_steady=tr.Vs*tr.T2;

    return tr;
}

struct scurve::traject scurve::dcc_period(struct traject tr, int debug){

    // Calculate the convec acc period, curve down. Document 5.3
    double jm=2*tr.Ds/tr.T3;
    double th=tr.T3/2;
    double vh=tr.Ve+jm*(th*th)/2; // Velocity at start of convex period. Velocity at first inflection point.

    if(debug){
        printf("TRAJECT CHECKS PERIOD DECCELERATION STAGE \n");
    }

    // Document page 4 (5.15)
    double s_convex= (vh+(tr.Ds*tr.Ds) / (3*jm) ) * tr.Ds / jm;
    if(debug){
        printf("displacement(s) dcc period convex (curve down): %f \n", s_convex);
    }

    // Document page 3 (5.3)
    double s_concave= (tr.Ve + (tr.Ds*tr.Ds) / (6*jm) ) * tr.Ds / jm;
    if(debug){
        printf("Check 0, using (s_concave=(tr.Ve+(tr.Ds*tr.Ds)/(6*jm))*tr.Ds/jm) + (s_convex=(vh+(tr.Ds*tr.Ds)/(3*jm))*tr.Ds/jm) \n");
        printf("displacement(s) dcc period concave (curve up): %f \n", s_concave);
        printf("displacement(s) total: %f \n", s_convex+s_concave);

    }

    // Document page 2 (5.7)
    tr.s_dcc_concave=tr.Ve*th+jm*(th*th*th)/6;
    // Document page 4 (5.11)
    tr.s_dcc_convex= (vh*th) + (tr.As*(th*th)/2) - (jm*(th*th*th)/6);
    if(debug){
        printf("Check 1, using (s_concave=tr.Ve*th+jm*(th*th*th)/6) + (s_convex=(vh*th)+(tr.Ds*(th*th)/2)-(jm*(th*th*th)/6)) \n");
        printf("displacement(s) total: %f \n", tr.s_dcc_concave+tr.s_dcc_convex);

    }

    // Document page 4 (5.16)
    tr.s_dcc=((tr.Vs*tr.Vs)-(tr.Ve*tr.Ve))/tr.Ds;
    if(debug){
        printf("Check 2, using ((tr.Vs*tr.Vs)-(tr.Vo*tr.Vo))/tr.As \n");
        printf("displacement(s) dcc total: %f \n", tr.s_dcc);
        printf(" \n");
    }

    // *************** Dcc period, curve down. ***************
    /*
    for(double t=0; t<=(tr.T3/2); t+=tr.timeinterval){

        double s=vh*t + tr.Ds*(t*t)/2 - jm*(t*t*t)/6;
        double v=vh + tr.Ds*t - jm*(t*t)/2;
        double a=tr.Ds-jm*t;

        if(debug){
            rtapi_print_msg(RTAPI_MSG_ERR,"s: %f \n", (tr.s_acc+tr.s_steady+(tr.s_dcc-(tr.s_dcc_concave+s))));
            rtapi_print_msg(RTAPI_MSG_ERR,"v: %f \n", v);
            rtapi_print_msg(RTAPI_MSG_ERR,"a: %f \n", a*-1);
            rtapi_print_msg(RTAPI_MSG_ERR,"jm: %f \n", jm*-1);
        }
        // xgraph=(tr.Ttot-th-t);
    }
    */

    // *************** Dcc period, curve up. ***************
    if(tr.Dcc_end>=(2*tr.Dcc_lineair)){       // tr.Acc_begin may be max 2*Acc_lineair at inflection point.
        tr.Dcc_end=(2*tr.Dcc_lineair)-0.1;    // added -0.1 to stay away from the inflection point.
    }
    tr.Dcc_end_time=tr.Dcc_end/jm;
    double time_to_remove=tr.T3-tr.Dcc_end_time;
    /*
    for(double t=Dcc_end_time; t<=(tr.T3/2); t+=tr.timeinterval){

        double s=tr.Ve*t+jm*(t*t*t)/6;  // Velocity end as start velocity dcc curve.
        double v=tr.Ve+jm*(t*t)/2;
        double a=jm*t;

        if(debug){
            rtapi_print_msg(RTAPI_MSG_ERR,"s: %f \n", tr.s_acc+tr.s_steady+(tr.s_dcc-s));
            rtapi_print_msg(RTAPI_MSG_ERR,"v: %f \n", v);
            rtapi_print_msg(RTAPI_MSG_ERR,"a: %f \n", a*-1);
            rtapi_print_msg(RTAPI_MSG_ERR,"jm: %f \n", jm);
        }
        // xgraph=(tr.Ttot-t);
    }
    */

    if(tr.T3>0){

        // If the acceleration end value is used, for the pathlenght we can do:
        if(tr.Dcc_end>0){
            double te=tr.Dcc_end_time;
            double lenght=tr.Ve*te+jm*(te*te*te)/6; // Part of a concave period that is not used.

            tr.s_dcc_concave-=lenght;   // Remove the not used part of the concave period.
            tr.s_dcc_convex= (vh*th) + (tr.As*(th*th)/2) - (jm*(th*th*th)/6); // Full convex period

            tr.L3=tr.s_dcc_concave+tr.s_dcc_convex;
            tr.Ltot=tr.L1+tr.L2+tr.L3;

            tr.T3=tr.T3-time_to_remove;
            tr.Ttot=tr.T1+tr.T2+tr.T3;

            // Calculate new end velocity:
            double Vend=tr.Ve+jm*(te*te)/2;
            if(debug){
                printf("Check 3, using acceleration end \n");
                printf("displacement(s) total: %f \n", tr.s_dcc_concave+ tr.s_dcc_convex);
                printf("velocity at end of curve: %f \n", Vend);
                printf(" \n");
            }
        }
    }

    // If the acceleration begin or end value is used, for the pathlenght we can do:
    if(tr.Acc_begin==0 && tr.Dcc_end==0){
        if(debug){
            printf("Total displacement scurve: %f \n", tr.s_acc+tr.s_steady+tr.s_dcc);
        }
    }
    if(tr.Acc_begin>0 && tr.Dcc_end>0){
        if(debug){
            printf("Total displacement scurve with acceleration begin & end total: %f \n", tr.s_acc_concave+
                   tr.s_acc_convex+
                   tr.s_steady+
                   tr.s_dcc_convex+
                   tr.s_dcc_concave);
        }
    } else
        if(tr.Acc_begin>0){
            if(debug){
                printf("Total displacement scurve with acceleration begin total: %f \n", tr.s_acc_concave+
                       tr.s_acc_convex+
                       tr.s_steady+
                       tr.s_dcc );
            }
        } else
            if(tr.Dcc_end>0){
                if(debug){
                    printf("Total displacement scurve with acceleration end total: %f \n", tr.s_acc+
                           tr.s_steady+
                           tr.s_dcc_convex+
                           tr.s_dcc_concave);
                }
            }
    return tr;
}

struct scurve::result scurve::request(struct traject tr, double timestamp){

    struct result r;

    if(timestamp>=0 && timestamp<(tr.T1/2)){
        // Concave period (curve up).
        r=request_acc_concave(tr,timestamp);
        return r;
    }
    if(timestamp==(tr.T1/2)){
        // At acc inflection point.
        r=request_acc_inflection(tr);
        return r;
    }
    if(timestamp>(tr.T1/2) && timestamp<tr.T1){
        // Convex period (curve down).
        r=request_acc_convex(tr,timestamp);
        return r;
    }
    if(timestamp==tr.T1){
        // End of acc period, start of steady period.
        r=request_acc_period(tr);
        return r;
    }
    if(timestamp>tr.T1 && timestamp<=(tr.T1+tr.T2)){
        // Steady period
        r=request_steady_period(tr,timestamp);
        return r;
    }
    if(timestamp==tr.T1+tr.T2){
        // Begin of dcc period, end of atspeed period.
        r=request_steady_period(tr,timestamp);
        return r;
    }
    if(timestamp>tr.T1+tr.T2 && timestamp<=(tr.T1+tr.T2+(tr.T3/2))){
        // Convex period (curve down).
        r=request_dcc_convex(tr,timestamp);
        return r;
    }
    if(timestamp==tr.T1+tr.T2+(0.5*tr.T3)){
        // At dcc inflection point.
        r=request_dcc_inflection(tr);
        return r;
    }
    if(timestamp>(tr.T1+tr.T2+(tr.T3/2)) && timestamp<tr.Ttot){
        // Concave period (curve up).
        r=request_dcc_concave(tr,timestamp);
        return r;
    }
    if(timestamp==tr.Ttot){
        // End of dcc period.
        r=request_total_period(tr);
        return r;
    }
    r.error=1;
    return r;
}

struct scurve::result scurve::request_acc_concave(struct traject tr, double t){ // curve up.

    struct result r;
    double jm=2*tr.As/tr.T1;    // Document page 2.

    r.s=tr.Vo*t+jm*(t*t*t)/6;   // s=displacement.
    r.v=tr.Vo+jm*(t*t)/2;       // v=velocity.
    r.a=jm*t;                   // a=acceleration.
    r.j=jm;                     // j=jerk.
    r.t=tr.Ttot;                // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement acc convave (curve up): %f \n", r.s);
    return r;
}

struct scurve::result scurve::request_acc_inflection(struct traject tr){
    struct result r;
    double jm=2*tr.As/tr.T1;                                        // Document page 2.

    r.s=(tr.Vo + (tr.As*tr.As) / (6*jm) ) * tr.As / jm;             // Document page 3 (5.3).
    r.v=(tr.Vs-tr.Vo)/2;                                            // At inflection point the velocity is half the (max velocity-initial velocity).
    r.a=2*tr.Acc_lineair;                                           // Acceleration at inflectionpoint is the lineair acceleration *2.
    r.j=0;                                                          // Jerk=0.
    r.t=tr.Ttot;                                                    // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement acc inflection: %f \n", r.s);
    return r;
}

struct scurve::result scurve::request_acc_convex(struct traject tr, double t){ // curve down.
    struct result r;
    double jm=2*tr.As/tr.T1;                                        // Document page 2.
    double th=tr.T1/2;                                              // Time of concave period.
    double vh=tr.Vo+jm*(th*th)/2;                                   // Velocity at start of convex period. Velocity at first inflection point.
    t-=tr.T1/2;                                                     // Remove time of the concave period.

    r.s=(vh*t + tr.As*(t*t)/2 - jm*(t*t*t)/6) + tr.s_acc_concave;   // Add the distance of the concave period.
    r.v=vh + tr.As*t - jm*(t*t)/2;
    r.a=tr.As-jm*t;
    r.j=jm*-1;
    r.t=tr.Ttot;                                                    // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement acc convex (curve down): %f \n", r.s);
    return r;
}

struct scurve::result scurve::request_acc_period(struct traject tr){
    struct result r;
    r.s=((tr.Vs*tr.Vs)-(tr.Vo*tr.Vo))/tr.As;                        // Document page 4 (5.16).
    r.v=tr.Vs;
    r.a=0;
    r.j=0;
    r.t=tr.Ttot;                                                    // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement acc period %f \n", r.s);
    return r;
}

struct scurve::result scurve::request_steady_period(struct traject tr, double t){
    struct result r;
    t-=tr.T1;                                                       // Remove time of acc period.
    r.s=(tr.Vs*t) + tr.s_acc;                                       // Add distance of acc period.
    r.v=tr.Vs;
    r.a=0;
    r.j=0;
    r.t=tr.Ttot;                                                    // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement steady period: %f \n", r.s);
    return r;
}

struct scurve::result scurve::request_dcc_convex(struct traject tr, double t){
    struct result r;

    double jm=2*tr.Ds/tr.T3;                                    // Jerk period dcc.
    double th=tr.T3/2;                                          // half of dcc period time.
    double vh=tr.Ve+jm*(th*th)/2;                               // Velocity at start of convex period. Velocity at first inflection point. See this as a mirror function.

    double tt=tr.Ttot-th-t;
    double s=vh*tt + tr.Ds*(tt*tt)/2 - jm*(tt*tt*tt)/6;

    r.s=tr.s_acc+tr.s_steady+(tr.s_dcc-(tr.s_dcc_concave+s));
    r.v=vh + tr.Ds*tt - jm*(tt*tt)/2;
    r.a=(tr.Ds-jm*tt)*-1;
    r.j=jm*-1;
    r.t=tr.Ttot;                                                // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement dcc convex (curve down): %f \n", r.s);
    return r;
}

struct scurve::result scurve::request_dcc_inflection(struct traject tr){
    struct result r;
    double jm=2*tr.As/tr.T3;                                        // Document page 2.

    double s=(tr.Ve + (tr.As*tr.As) / (6*jm) ) * tr.As / jm;        // Document page 3 (5.3).

    r.s=tr.s_acc+tr.s_steady+s;
    r.v=(tr.Vs-tr.Ve)/2;                                            // At inflection point the velocity is half the (max velocity-initial velocity).
    r.a=2*tr.Dcc_lineair;                                           // Acceleration at inflectionpoint is the lineair acceleration *2.
    r.j=0;                                                          // Jerk=0.
    r.t=tr.Ttot;                                                    // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement dcc inflection: %f \n", r.s);
    return r;
}

struct scurve::result scurve::request_dcc_concave(struct traject tr, double t){

    struct result r;
    double jm=2*tr.Ds/tr.T3;                                        // Jerk period dcc.

    double tt=tr.Ttot-t;                                            // Invert time.
    double s=tr.Ve*tt+jm*(tt*tt*tt)/6;                              // Velocity end as start velocity dcc curve.

    r.s=tr.s_acc+tr.s_steady+(tr.s_dcc-s);
    r.v=tr.Ve+jm*(tt*tt)/2;
    r.a=(jm*tt)*-1;
    r.j=jm;
    r.t=tr.Ttot;                                                    // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement dcc concave (curve up): %f \n", r.s);
    return r;
}

struct scurve::result scurve::request_total_period(struct traject tr){
    struct result r;

    r.s=tr.s_acc+tr.s_steady+tr.s_dcc;
    r.v=tr.Ve;
    r.a=0;
    r.j=0;
    r.t=tr.Ttot;                                                    // Add the total traject time.
    //rtapi_print_msg(RTAPI_MSG_ERR,"displacement total: %f \n", r.s);
    return r;
}




























