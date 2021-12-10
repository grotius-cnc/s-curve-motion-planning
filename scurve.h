#ifndef SCURVE_H
#define SCURVE_H

#include "external_var.h"
#include "math.h"           /* Used for pow and sqrt */
#include "iostream"

class scurve
{
public:
    scurve();

    struct traject {

        double Vo;              //start velocity
        double Vs;              //max velocity
        double Ve;
        double Acc_lineair;     //lineair acceleration, mm/sec^2, acceleration stage.
        double Acc_begin;       //Stepping into a curve at a certian acceleration value.

        double Dcc_lineair;     //lineair deacceleration, mm/sec^2, deacceleration stage.
        double Dcc_end;
        //! max acceleration at inflection point, acc_lin*2, mm/sec^2
        double As;              //max acceleration at inflection point, acc_lin*2, mm/sec^2
        double Ds;              //max acceleration at inflection point, dcc_lin*2, mm/sex^2

        double T0;              //Start time
        double T1;              //total Acc time.
        double T2;              //total atspeed time.
        double T3;              //total Dcc time.
        double Ttot;            //total traject time, T1+T2+T3

        double L1;              //acceleration lenght
        double L2;              //atspeed lenght.
        double L3;              //deacceleration lenght
        double Ltot;            //total traject lenght, L1+L2+L3

        double s_acc_concave;   //displacement convave period acc, curve up, document page 1.
        double s_acc_convex;    //displacement convex period acc, curve down.
        double s_acc;           //displacement acc period, concave+convex
        double s_steady;        //displacement atspeed/steady period
        double s_dcc_convex;    //displacement convex period dcc, curve down. (one position for last curve segment)
        double s_dcc_concave;   //displacement convave period dcc, curve up. (last curve segment)
        double s_dcc;           //dispalcement dcc period, convex+concave.

        double Acc_start_time;  //when acc_begin>0 calculate the time(t)
        double Dcc_end_time;    //when dcc_end>0 calculate the time(t)
    };

    struct result {
        // Displacement
        double s;
        // Velocity
        double v;
        // Acceleration
        double a;
        // Jerk
        double j;
        // Total traject time
        double t;
        // Error
        bool error;
    };

    void demo_traject();

    // Driver function decleration:
    struct result request_trajectcalculator_scurve(double velmax, double accmax, double dccmax, double accbegin, double dccend, double vbegin, double vend, double pathlenght, double timestamp, int debug);

    // Traject lenghs and times declarations:
    struct traject acc_period(struct traject tr, int debug);
    struct traject steady_period(struct traject tr, int debug);
    struct traject dcc_period(struct traject tr, int debug);

    // Sub function declerations to reqeust values at a certain timestamp:
    struct result request(struct traject tr, double timestamp); // Central function to select one of the request functions, time related selection.
    struct result request_acc_concave(struct traject tr, double t);
    struct result request_acc_inflection(struct traject tr);
    struct result request_acc_convex(struct traject tr, double t);
    struct result request_acc_period(struct traject tr);
    struct result request_steady_period(struct traject tr, double t);
    struct result request_dcc_convex(struct traject tr, double t);
    struct result request_dcc_inflection(struct traject tr);
    struct result request_dcc_concave(struct traject tr, double t);
    struct result request_total_period(struct traject tr);
};

#endif // SCURVE_H
