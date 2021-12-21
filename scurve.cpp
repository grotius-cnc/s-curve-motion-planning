#include "scurve.h"

Scurve::Scurve()
{
}

//! Example usage of lineair stage.
void Scurve::example_lineair(){
    double ve=5;
    double s=10;

    double at_time=0;
    double ct=0;
    double st=0;
    int r=0;

    r+=scurve_lineair(at_time,ve,s,st,ct);
    if(!r){
        std::cout<<"Curve time[ct]:"<<ct<<std::endl;
        std::cout<<"Dispalcement[st] at [at_time]:"<<st<<std::endl;

        for(double i=0; i<ct; i+=0.1){
            r+=scurve_lineair(i,ve,s,st,ct);
            std::cout<<"Dispalcement[st]:"<<st<<" at_time[i]"<<i<<std::endl;
        }
    }
}

//! Inputs:
//! at_time=request displacment at a certain time stamp.
//! ve=velocity
//! s=total displacement
//! Outputs:
//! st=displacment at_time request.
//! ct=total curve time.
int Scurve::scurve_lineair(double at_time, double ve, double s, double &st, double &ct){
    int r=0;

    //! Limits.
    if(ve==0){
        std::cout<<"vel can not be 0"<<std::endl;
        return 1;
    }

    //! Formula : s=v*t
    ct=s/ve;        //! Curve time [ct]
    ct=std::abs(ct);     //! Used when velocity is negative.

    st=ve*at_time;  //! Displacement [st] at timestamp [at_time]

    if(ct<0){
        std::cout<<"lineair curve time <0"<<std::endl;
    }
    if(st<0){
        std::cout<<"lineair curve displacment <0"<<std::endl;
    }

    return r;
}

//! Example usage of scurve.
void Scurve::example_scurve(){
    int sct=0;
    //! Acceleration start.
    double acs=0;
    //! Acceleration end.
    double ace=0;
    //! Start velocity
    double vo=0;
    //! End velocity
    double ve=5;
    //! Max acceleration.
    double am=2;

    int r=0;
    double sr,vr,ar,ct;
    double at_time=0;
    //! Request ct, curve time.
    scurve_acc_dcc(sct,vo, ve, am, acs, ace, at_time, sr, vr, ar, ct);
    if(ct<0){
        std::cout<<"curve time<0"<<std::endl;
    }
}

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
//!
int Scurve::scurve_acc_dcc(int sct, double vo, double ve, double am, double acs, double ace, double at_time, double &sr, double &vr, double &ar, double &ct){

    bool debug=0;

    //! ***** = Addons for scurve mirror effect.
    if(sct==1){
        //! ***** mirror acceleration start, end.
        double temp_acs=acs;
        acs=ace;
        ace=temp_acs;

        //! ***** Mirror ve & vo.
        double temp_ve=ve;
        ve=vo;
        vo=temp_ve;

        //! ***** Mirror time.
        at_time=ct-at_time;
        if(at_time<0){at_time=0;}
    }

    //! Minimal maximum acceleration.
    if(am<0.1){
        am=0.1;

        std::cout<<"am set to:"<<am<<std::endl;
    }
    //! Limit acceleration start value.
    if(acs<0){
        acs=0;
        std::cout<<"acs set to:"<<ace<<std::endl;
    }
    if(acs>(2*am)){
        acs=2*am;
        std::cout<<"acs limited to:"<<acs<<std::endl;
    }
    //! Limit acceleration end value.
    if(ace<0){
        ace=0;
        std::cout<<"ace set to:"<<ace<<std::endl;
    }
    if(ace>2*am){
        ace=2*am;
        std::cout<<"ace limited to:"<<ace<<std::endl;
    }
    //! For acc curve velocity end > velocity start.
    if(vo>=ve){
        std::cout<<"impossible situation for acc curve. vo>ve"<<std::endl;
        ve=vo+0.01;
    }

    //! Displacement concave period.
    double sce=0;
    //! End velocity concave period.
    double vece=0;
    //! Displacement convex period.
    double scx=0;
    //! End velocity convex period.
    double vecx=0;
    //! Max acceleration at inflection point. [2*A]
    double as=2*am;
    //! Time acc period. Document page 3, observations : T=2*Delta_v/as
    double t1=2*(ve-vo)/as;
    //! Lenght acc period. Document page 4 (5.16)
    double l1=((ve*ve)-(vo*vo))/as;
    //! Max jerk. Document page 2.
    double jm=2*as/t1;

    //! Internals.
    double th=0, vh=0, ts=0, te=0, t=0;

    //! If scurve has acceleration start value. Find start time[t].
    if(acs>0){
        ts=acs/jm;                                      //! Time[t] on concave period. a=jm*t;
    } else {
        ts=0;
    }
    //! Curve has acceleration end value. Find end time[t].
    if(ace>0){
        te=(as-ace)/jm;                                 //! Time[t] on concvex period. a=jm*t;
    } else {
        te=t1/2;
    }

    //! Concave period.
    t=ts;                                               //! at start of curve.
    double sa=vo*t+jm*(t*t*t)/6;                        //! Netto displacment concave front.
    t=t1/2;                                             //! at end of curve.
    vece=vo+jm*(t*t)/2;                                 //! velocity end.
    double sb=vo*t+jm*(t*t*t)/6;                        //! Bruto displacement concave front.
    sce=sb-sa;                                          //! Netto displacment convace back.


    //! Convex period.
    th=t1/2;
    vh=vo+jm*(th*th)/2;
    t=te;
    vecx=vh + as*t - jm*(t*t)/2;                        //! Convex velocity end.
    scx=vh*t + as*(t*t)/2 - jm*(t*t*t)/6;               //! Netto displacement convex front.
    t=t1/2;
    double sc=vh*t + as*(t*t)/2 - jm*(t*t*t)/6;         //! Bruto displacement.
    double sd=sc-scx;                                   //! Netto displacement convex back.

    double stn=0;                                       //! Curve total netto displacment, excl. acc & dcc start value displacments.
    stn+=sce;
    stn+=scx;

    double concave=(t1/2)-ts;
    double convex=te;
    ct=concave+convex;                                  //! Netto curve time [ct]
    if(at_time<=ct){
        if(at_time<=concave){
            //! ts is relative startpos if start acc is used.
            t=at_time+ts;
            sr=vo*t+jm*(t*t*t)/6;                       //! Displacement.
            sr-=vo*ts+jm*(ts*ts*ts)/6;                  //! Substract front acc.
            vr=vo+jm*(t*t)/2;                           //! Velocity.
            ar=jm*t;                                    //! Acceleration.

            if(sct==1){                                 //! **** Scurve type : dcc.
                ar=-std::abs(ar);                            //! **** Dcc curve.
                sr=stn-sr;                              //! **** Dcc curve, mirror displacment.
            }

        }
        if(at_time>concave){
            t=at_time-concave;

            th=t1/2;
            vh=vo+jm*(th*th)/2;                         //! Velocity at start of convex period. Velocity at first inflection point.

            sr=vh*t + as*(t*t)/2 - jm*(t*t*t)/6;
            sr+=sce;                                    //! Add displacement concave period.
            vr=vh + as*t - jm*(t*t)/2;
            ar=as-jm*t;

            if(sct==1){                                 //! **** Scurve type : dcc.
                ar=-std::abs(ar);                            //! ***** Dcc curve.
                sr=stn-sr;                              //! **** Dcc curve, mirror displacment.
            }
        }
    }

    //! This now does nothing. I leave it here, just for info for other users.
    if(debug){
        std::cout<<"Check displacements"<<std::endl;
        std::cout<<"acc period displacement l1:"<<l1<<std::endl;
        std::cout<<""<<std::endl;
        std::cout<<"1.concave start acc dist :"<<sa<<std::endl;
        std::cout<<"2.concave dist           :"<<sce<<std::endl;
        std::cout<<"3.convex dist            :"<<scx<<std::endl;
        std::cout<<"4.convex end acc dist    :"<<sd<<std::endl;
        std::cout<<"acc period displacement sum 1-4:"<<sa+sce+scx+sd<<std::endl;

        std::cout<<"Check time"<<std::endl;
        std::cout<<"acc period t1:"<<t1<<std::endl;
        std::cout<<""<<std::endl;
        std::cout<<"1.convex+concave period  :"<<ct<<std::endl;
    }

    return 0;
}

//! c module interface.
extern "C" int scurve_lineair(double at_time, double ve, double s, double &st, double &ct){
    int r=Scurve().scurve_lineair(at_time, ve, s, st, ct);
    return r;
}

//! c module interface.
extern "C" int scurve_acc_dcc(int sct, double vo, double ve, double am, double acs, double ace, double at_time, double &sr, double &vr, double &ar, double &ct){
    int r=Scurve().scurve_acc_dcc(sct, vo, ve, am, acs, ace, at_time, sr, vr, ar, ct);
    return r;
}
