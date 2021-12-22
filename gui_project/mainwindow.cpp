#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <string>
#include <math.h>
#include <iostream>

using namespace occ;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    OpencascadeWidget = new Opencascade(this);
    ui->gridLayout_opencascade->addWidget(OpencascadeWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_create_motion_block_pressed(){
    std::cout.precision(3);
    Handle(AIS_Shape) awire;
    std::vector<gp_Pnt> pvec_v, pvec_s, pvec_a;

    //! Velocity max.
    double vs=ui->doubleSpinBox_mb_velocity_max->value();
    //! Acceleration max.
    double am=ui->doubleSpinBox_mb_acc_max->value();

    //! Pathlenght.
    double ltot=ui->doubleSpinBox_mb_pathlenght->value();

    //! Velocity begin.
    double vo=ui->doubleSpinBox_mb_velocity_start->value();
    //! Acceleration begin.
    double acs=ui->doubleSpinBox_mb_acc_begin->value();

    //! Velocity end.
    double ve=ui->doubleSpinBox_mb_velocity_end->value();
    //! Acceleration end.
    double ace=ui->doubleSpinBox_mb_acc_end->value();

    //! Curve request at time [t]
    double at_time=0;

    //! Results:
    //! Current displacement.
    double sr=0;
    //! Current velocity.
    double vr=0;
    //! Current acceleration.
    double ar=0;
    //! Total curve time.
    double ct=0;
    //! Displacment opencascade scale.
    double to_scale=0.1;

    int r=0;

    //! Request curve time [ct]
    r+=motion_block(vs, am, vo, acs, ltot, ve, ace, at_time,sr,vr,ar,ct);
    if(!r){std::cout<<"error"<<std::endl;}

    for(double t=0; t<ct; t+=0.1){
        auto start = std::chrono::high_resolution_clock::now();
        r+=motion_block(vs, am, vo, acs, ltot, ve, ace, t,sr,vr,ar,ct);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        std::cout << "Time taken by function nanoseconds: " << duration.count() << " milliseconds:" << duration.count()*0.000001 <<std::endl;

        std::cout<<std::fixed<<"at_time:"<<t<<" sr:"<<sr<<" vr:"<<vr<<" ar:"<<ar<<" ct:"<<ct<<std::endl;

        //! The avarage function time = 1500 us.
        //! ns*0.000001=ms 0.0015 ms. This is ok for 1ms servo thread.

        if(!r){
            pvec_v.push_back({t,vr,0});
            pvec_s.push_back({t,sr*to_scale,0});
            pvec_a.push_back({t,ar,0});
        }
    }

    if(!r){
        if(pvec_v.size()>0){
            awire=draw_primitives().draw_3d_line_wire(pvec_v);
            awire=draw_primitives().colorize(awire,Quantity_NOC_BLACK,0);
            OpencascadeWidget->show_shape(awire);
        }
        if(pvec_s.size()>0){
            awire=draw_primitives().draw_3d_line_wire(pvec_s);
            awire=draw_primitives().colorize(awire,Quantity_NOC_BLUE,0);
            OpencascadeWidget->show_shape(awire);
        }
        if(pvec_a.size()>0){
            awire=draw_primitives().draw_3d_line_wire(pvec_a);
            awire=draw_primitives().colorize(awire,Quantity_NOC_RED,0);
            OpencascadeWidget->show_shape(awire);

            OpencascadeWidget->Redraw();
        }
    }
}

//! Inputs:
//! vs=velocity max.
//! am=acceleraton max.
//! po=position begin.
//! vo=velocity begin.
//! acs=acceleration begin.
//! pe=position end.
//! ve=velocity end.
//! ace=acceleration end.
//!
//! at_time=request scurve at time stamp [t].
//!
//! Output:
//! vsrx=sampled velocity max of motion block if curves don't fit the displacment. Initialized as [vs].
//! sr=current velocity.
//! vr=current velocity.
//! ar=current acceleration.
//! total_ct=total traject time.
//!
int MainWindow::motion_block(double vs, double am, double vo, double acs, double ltot, double ve, double ace, double at_time, double &sr,double &vr, double &ar ,double &ct){
    int r=0;

    //! Limits.
    if(vo>vs){
        vo=vs;
    }
    if(ve>vs){
        ve=vs;
    }
    if(ltot<=0){
        std::cout<<"pahtlenght is zero or less"<<std::endl;
        return 1;
    }
    if(vs<=0){
        std::cout<<"velocity max is zero or less"<<std::endl;
        return 1;
    }

    //! Internals.
    double l1=0,l2=0,l3=0,t1=0,t2=0,t3=0,t=0,dummy=0;;

    //! ***** Acc stage.
    //! Request curve time.
    r+=scurve_acc_dcc(0,vo, vs, am, acs, 0, t, sr, vr, ar, t1, l1);
    sr=0,vr=0,ar=0;

    //! ***** Dcc stage.
    //! Request curve time.
    r+=scurve_acc_dcc(1,vs, ve, am, 0, ace ,t, sr, vr, ar, t3, l3);
    sr=0,vr=0,ar=0;

    //! ***** Line stage.
    //! Request curve time.
    l2=ltot-(l1+l3);
    if(l2>=0){
        r+=scurve_lineair(t,vs,l2,t2,dummy);
    }

    //! Motion can not reach max velocity [vs].
    if((l1+l3)>ltot){
        //! Sample the [vs] of the curves until they fit ltot. Curves can have different lenghts because of acc start, acc end value.
        while((l1+l3)>ltot){
            vs-=0.1;
            r+=scurve_acc_dcc(0,vo, vs, am, acs, 0, t, sr, vr, ar, t1, l1);
            r+=scurve_acc_dcc(1,vs, ve, am, 0, ace ,t, sr, vr, ar, t3, l3);
        }
        //! Calculate new l2.
        l2=ltot-(l1+l3);
        r+=scurve_lineair(t,vs,l2,t2,dummy);
    }

    //! Total s-curve time. [ct]
    ct=t1+t2+t3;

    if(at_time<=t1){
        t=at_time;
        r+=scurve_acc_dcc(0,vo, vs, am, acs, 0, t, sr, vr, ar, t3, l1);
        return 0;
    }

    if(at_time>t1 && at_time<(t1+t2)){
        t=at_time;
        t=at_time-t1;
        r+=scurve_lineair(t,vs,l2,t2,sr);
        vr=vs;                                      //! Lineair stage has max velocity [vs].
        sr+=l1;
        return 0;

    }

    if(at_time>=(t1+t2)){
        t=at_time-t1-t2;
        r+=scurve_acc_dcc(1,vs, ve, am, 0, ace ,t, sr, vr, ar, t3, l3);
        sr+=l1+l2;
        return 0;
    }

    return 1;
}

void MainWindow::on_pushButton_show_lin_curve_pressed()
{
    std::cout.precision(3);
    double vs=ui->doubleSpinBox_lin_velocity->value();
    double s=ui->doubleSpinBox_lin_displacment->value();

    double at_time=0;
    double ct=0;
    double sr=0;
    double vr=0;
    double ar=0;
    int r=0;

    r+=scurve_lineair(at_time,vs,s,ct,sr);
    if(!r){
        //! Draw the full traject.
        Handle(AIS_Shape) aLine;
        aLine=draw_primitives().draw_3d_line({0,vs,0},{ct,vs,0});
        aLine=draw_primitives().colorize(aLine,Quantity_NOC_BLACK,0);
        OpencascadeWidget->show_shape(aLine);

        for(double i=0; i<ct; i+=0.1){
            r+=scurve_lineair(i,vs,s,sr,ct);
            ar=0;   //! Zero acceleration during steady stage.
            vr=vs;  //! Velocity = Max velocity.
            std::cout<<std::fixed<<"at_time:"<<i<<" sr:"<<sr<<" vr:"<<vr<<" ar:"<<ar<<" ct:"<<ct<<std::endl;
        }
    }
}

//! Inputs:
//! at_time=request displacment at a certain time stamp.
//! vs=velocity max
//! s=total displacement
//! Outputs:
//! ct=total curve time.
//! sr=displacment at_time request.
int MainWindow::scurve_lineair(double at_time, double vs, double s, double &ct, double &sr){
    int r=0;

    //! Limits.
    if(vs==0){
        std::cout<<"vel can not be 0"<<std::endl;
        return 1;
    }

    //! Formula : s=v*t
    ct=s/vs;        //! Curve time [ct]
    ct=std::abs(ct);     //! Used when velocity is negative.

    sr=vs*at_time;  //! Displacement [st] at timestamp [at_time]

    if(ct<0){
        std::cout<<"lineair curve time <0"<<std::endl;
        //return 1;
    }
    if(sr<0){
        std::cout<<"lineair curve displacment <0"<<std::endl;
        //return 1;
    }

    return r;
}

void MainWindow::on_pushButton_show_acc_dcc_curve_pressed()
{
    int sct=ui->comboBox_scruve_type->currentIndex();
    //! Acceleration start.
    double acs=ui->doubleSpinBox_acceleration_begin->value();
    //! Acceleration end.
    double ace=ui->doubleSpinBox_acceleration_end->value();
    //! Start velocity
    double vo=ui->doubleSpinBox_velocity_start->value();
    //! End velocity
    double ve=ui->doubleSpinBox_velocity_end->value();
    //! Max acceleration.
    double am=ui->doubleSpinBox_acceleration_max->value();

    int r=0;
    std::cout.precision(3);
    Handle(AIS_Shape) awire;
    std::vector<gp_Pnt> pvec_v, pvec_s, pvec_a;

    double sr,vr,ar,ct,cs;
    double at_time=0.0; //! Pointvec 0.0 will interfere with opencascade, duplicate point.
    //! Request ct, curve time.
    scurve_acc_dcc(sct,vo, ve, am, acs, ace, at_time, sr, vr, ar, ct, cs);
    if(ct<0){
        std::cout<<"curve time<0"<<std::endl;
    }

    for(double t=0; t<ct; t+=0.01){
        r+=scurve_acc_dcc(sct,vo, ve, am, acs, ace, t, sr, vr, ar, ct, cs);
        if(!r){
            pvec_v.push_back({t,vr,0});
            pvec_s.push_back({t,sr,0});
            pvec_a.push_back({t,ar,0});
            std::cout<<std::fixed<<"at_time:"<<t<<" sr:"<<sr<<" vr:"<<vr<<" ar:"<<ar<<" ct:"<<ct<<std::endl;
        } else {std::cout<<"error from scurve function"<<std::endl;}
    }
    r+=scurve_acc_dcc(sct,vo, ve, am, acs, ace, ct, sr, vr, ar, ct, cs);
    if(!r){
        pvec_v.push_back({ct,vr,0});
        pvec_s.push_back({ct,sr,0});
        pvec_a.push_back({ct,ar,0});
        std::cout<<std::fixed<<"at_time:"<<ct<<" sr:"<<sr<<" vr:"<<vr<<" ar:"<<ar<<" ct:"<<ct<<std::endl;
    } else {std::cout<<"error from scurve function"<<std::endl;}

    if(!r){
        if(pvec_v.size()>0){
            awire=draw_primitives().draw_3d_line_wire(pvec_v);
            awire=draw_primitives().colorize(awire,Quantity_NOC_BLACK,0);
            OpencascadeWidget->show_shape(awire);
        }
        if(pvec_s.size()>0){
            awire=draw_primitives().draw_3d_line_wire(pvec_s);
            awire=draw_primitives().colorize(awire,Quantity_NOC_BLUE,0);
            OpencascadeWidget->show_shape(awire);
        }
        if(pvec_a.size()>0){
            awire=draw_primitives().draw_3d_line_wire(pvec_a);
            awire=draw_primitives().colorize(awire,Quantity_NOC_RED,0);
            OpencascadeWidget->show_shape(awire);

            OpencascadeWidget->Redraw();
        }
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
//! sr=current displacment at [t]
//! vr=current_velocity at [t]
//! ar=current acceleration at [t]
//!
//! ct=netto curve time [t] excluding acc start time, acc end time.
//!
int MainWindow::scurve_acc_dcc(int sct, double vo, double ve, double am, double acs, double ace, double at_time, double &sr, double &vr, double &ar, double &ct, double &cs){

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
    cs=stn;                                             //! Curve displacment

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

    //! This does nothing now, but i leave it here just for info.
    bool debug=0;
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

void MainWindow::on_pushButton_clear_dcc_curve_pressed()
{
    OpencascadeWidget->Remove_all();
}

void MainWindow::on_pushButton_clear_lin_curve_pressed()
{
    OpencascadeWidget->Remove_all();
}

void MainWindow::on_pushButton_mb_clear_pressed()
{
    OpencascadeWidget->Remove_all();
}

void MainWindow::on_doubleSpinBox_velocity_start_valueChanged(double arg1)
{
    on_pushButton_show_acc_dcc_curve_pressed();
}

void MainWindow::on_doubleSpinBox_velocity_end_valueChanged(double arg1)
{
    on_pushButton_show_acc_dcc_curve_pressed();
}

void MainWindow::on_doubleSpinBox_acceleration_max_valueChanged(double arg1)
{
    on_pushButton_show_acc_dcc_curve_pressed();
}

void MainWindow::on_doubleSpinBox_acceleration_begin_valueChanged(double arg1)
{
    on_pushButton_show_acc_dcc_curve_pressed();
}

void MainWindow::on_comboBox_scruve_type_currentIndexChanged(int index)
{
    //! Swap velocity values to avoid impossible situation.
    double vo=ui->doubleSpinBox_velocity_start->value();
    double ve=ui->doubleSpinBox_velocity_end->value();
    ui->doubleSpinBox_velocity_start->setValue(ve);
    ui->doubleSpinBox_velocity_end->setValue(vo);
}

void MainWindow::on_doubleSpinBox_lin_velocity_valueChanged(double arg1)
{
    on_pushButton_show_lin_curve_pressed();
}

void MainWindow::on_doubleSpinBox_lin_displacment_valueChanged(double arg1)
{
    on_pushButton_show_lin_curve_pressed();
}

void MainWindow::on_doubleSpinBox_mb_velocity_start_valueChanged(double arg1)
{
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_velocity_end_valueChanged(double arg1)
{
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_velocity_max_valueChanged(double arg1)
{
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_acc_max_valueChanged(double arg1)
{
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_acc_begin_valueChanged(double arg1)
{
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_acc_end_valueChanged(double arg1)
{
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_pathlenght_valueChanged(double arg1)
{
    on_pushButton_create_motion_block_pressed();
}
