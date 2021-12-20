// Example code.
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <string>
#include <math.h>
#include <iostream>

using namespace Eigen;
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

void MainWindow::short_acc(){


}

void MainWindow::on_toolButton_show_graph_pressed()
{
    //! Acceleration start.
    double acs=ui->doubleSpinBox_acceleration_begin->value();
    //! Acceleration end.
    double ace=ui->doubleSpinBox_acceleration_end->value();
    //! Start velocity
    double vo=ui->doubleSpinBox_velocity_start->value();
    //! End velocity
    double ve=ui->doubleSpinBox_velocity_end->value();
    //! Max velocity.
    double vs=ui->doubleSpinBox_velocity_max->value();
    //! Max acceleration.
    double am=ui->doubleSpinBox_acceleration_max->value();

    int r=0;
    std::cout.precision(3);
    Handle(AIS_Shape) awire;
    std::vector<gp_Pnt> pvec_v, pvec_s, pvec_a;

    double sr,vr,ar,ct;
    double at_time=0.1; //! Pointvec 0.0 will interfere with opencascade, duplicate point.
    //! Request ct, curve time.
    r+=scurve_acc(vo, vs, ve, am, acs, ace, at_time, sr, vr, ar, ct);

    for(double t=0; t<ct; t+=0.01){
        r+=scurve_acc(vo, vs, ve, am, acs, ace, t, sr, vr, ar, ct);
        if(!r){
            pvec_v.push_back({t,vr,0});
            pvec_s.push_back({t,sr,0});
            pvec_a.push_back({t,ar,0});
            //std::cout<<std::fixed<<"at_time:"<<t<<" sr:"<<sr<<" vr:"<<vr<<" ar:"<<ar<<" ct:"<<ct<<std::endl;
        } else {std::cout<<"error from scurve function"<<std::endl;}
    }
    r+=scurve_acc(vo, vs, ve, am, acs, ace, ct, sr, vr, ar, ct);
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
int MainWindow::scurve_acc(double vo, double vs, double ve, double am, double acs, double ace, double at_time, double &sr, double &vr, double &ar, double &ct){

    if(acs>(2*am)){
        acs=2*am;
        std::cout<<"acs limited to:"<<acs<<std::endl;
        return 1;
    }
    if(ace>2*am){
        ace=2*am;
        std::cout<<"ace limited to:"<<ace<<std::endl;
        return 1;
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
    double t1=2*(vs-vo)/as;
    //! Lenght acc period. Document page 4 (5.16)
    double l1=((vs*vs)-(vo*vo))/as;
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

    double concave=(t1/2)-ts;
    double convex=te;
    ct=concave+convex;                                  //! Netto curve time [ct]
    if(at_time<=ct){
        if(at_time<concave){
            //! ts is relative startpos if start acc is used.
            t=at_time+ts;
            sr=vo*t+jm*(t*t*t)/6;                       //! Displacement.
            sr-=vo*ts+jm*(ts*ts*ts)/6;                  //! Substract front acc.
            vr=vo+jm*(t*t)/2;                           //! Velocity.
            ar=jm*t;                                    //! Acceleration.
        }
        if(at_time>concave){
            t=at_time-concave;

            th=t1/2;
            vh=vo+jm*(th*th)/2;                         //! Velocity at start of convex period. Velocity at first inflection point.

            sr=vh*t + as*(t*t)/2 - jm*(t*t*t)/6;
            sr+=sce;                                    //! Add displacement concave period.
            vr=vh + as*t - jm*(t*t)/2;
            ar=as-jm*t;
        }
    }

    /*
    //! Control.
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
    std::cout<<"1.convex+concave period  :"<<ct<<std::endl;*/

    return 0;
}


void MainWindow::on_doubleSpinBox_acceleration_end_valueChanged(double arg1)
{
    on_toolButton_show_graph_pressed();
}

void MainWindow::on_doubleSpinBox_acceleration_begin_valueChanged(double arg1)
{
    on_toolButton_show_graph_pressed();
}

void MainWindow::on_doubleSpinBox_velocity_max_valueChanged(double arg1)
{
    on_toolButton_show_graph_pressed();
}

void MainWindow::on_doubleSpinBox_acceleration_max_valueChanged(double arg1)
{
    on_toolButton_show_graph_pressed();
}

void MainWindow::on_doubleSpinBox_velocity_start_valueChanged(double arg1)
{
    on_toolButton_show_graph_pressed();
}

void MainWindow::on_doubleSpinBox_velocity_end_valueChanged(double arg1)
{
    on_toolButton_show_graph_pressed();
}

void MainWindow::on_pushButton_clear_pressed()
{
    OpencascadeWidget->Remove_all();
}
