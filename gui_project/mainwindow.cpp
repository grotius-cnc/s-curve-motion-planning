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

    double t=0;

    //! A motion.
    bool ok=1;
    while(ok){
        auto start = std::chrono::high_resolution_clock::now();
        RESULT r=motion(vs, am, vo, acs, ltot, ve, ace, t);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        std::cout << "Time taken by function nanoseconds : ~" << duration.count() << " milliseconds: ~" << duration.count()*0.000001 <<std::endl;

        t+=0.1;
        if(t>=r.ct){
            ok=0;
        }

        std::cout<<std::fixed<<"at_time:"<<t<<" sr:"<<r.sr<<" vr:"<<r.vr<<" ar:"<<r.ar<<" ct:"<<r.ct<<std::endl;

        if(!r.error){
            pvec_v.push_back({t,r.vr,0});
            pvec_s.push_back({t,r.sr*0.1/*scale*/,0});
            pvec_a.push_back({t,r.ar,0});
        }
    }
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

MainWindow::RESULT MainWindow::motion(double vs, double am, double vo, double acs, double ltot, double ve, double ace, double at_time){

    RESULT r;
    double t=0;

    if(ltot<=0){
        r.error=1;
        return r;
    }
    if(ve>vs){
        ve=vs;
    }

    //! Priority, starting at acc<0, controlled stop, goto to vs.
    if(acs<=0 && vs<vo){
        t=at_time;
        if(ve<vs){vs=ve;}
        r=scurve_acc_dcc(1,vo, vs, am, acs, ace ,t);
        std::cout<<"mode 0"<<std::endl;
        return r;
    }

    //! Priority, starting at acc>=0, controlled stop, goto to vs.
    if(acs>0 && vs<vo){
        //! Get curve time.
        t=0;
        RESULT a,b;
        double ct=0;

        if(ve<vs){vs=ve;}
        double gain=acs;

        a=scurve_acc_dcc(0,vo, vo+gain, am, acs, 0/*ace*/ ,t);
        b=scurve_acc_dcc(1,vo+gain, vs, am, 0/*acs*/, ace ,t);
        ct=a.ct+b.ct;
        /*
        std::cout<<"l0:"<<a.cs<<std::endl;
        std::cout<<"l1:"<<b.cs<<std::endl;
        std::cout<<"ltot:"<<a.cs+b.cs<<std::endl;
        std::cout<<"t0:"<<a.ct<<std::endl;
        std::cout<<"t1:"<<b.ct<<std::endl;
        std::cout<<"ttot:"<<a.ct+b.ct<<std::endl;*/

        t=at_time;

        if(t<a.ct){
            r=scurve_acc_dcc(0,vo, vo+gain, am, acs, 0/*ace*/ ,t);
        }
        if(t>=a.ct){
            t-=a.ct;
            r=scurve_acc_dcc(1,vo+gain, vs, am, 0/*acs*/, ace ,t);
            r.sr+=a.cs;
        }

        r.ct=ct;
        // std::cout<<"mode 1"<<std::endl;
        return r;
    }

    //! Normal curve, starting with acc>0
    if(acs>=0 && vs>=vo && vs>=ve){
        //! Get curve time.
        t=0;
        RESULT a,b,c;
        double ct=0;
        a=scurve_acc_dcc(0,vo, vs, am, acs, 0/*ace*/ ,t);
        c=scurve_acc_dcc(1,vs, ve, am, 0/*acs*/, ace ,t);

        double lb=ltot-(a.cs+c.cs);
        b=scurve_lineair(0,vs,lb);
        ct=a.ct+b.ct+c.ct;

        /*
        std::cout<<"l0:"<<a.cs<<std::endl;
        std::cout<<"l1:"<<b.cs<<std::endl;
        std::cout<<"l2:"<<c.cs<<std::endl;
        std::cout<<"ltot:"<<a.cs+b.cs+c.cs<<std::endl;
        std::cout<<"t0:"<<a.ct<<std::endl;
        std::cout<<"t1:"<<b.ct<<std::endl;
        std::cout<<"t2:"<<c.ct<<std::endl;
        std::cout<<"ttot:"<<a.ct+b.ct+c.ct<<std::endl;*/

        if(lb<0){
            // std::cout<<"lin stage<0!"<<std::endl;
            //! Sample curve to fit ltot.
            while(a.cs+c.cs>ltot){
                vs-=0.1;
                a=scurve_acc_dcc(0,vo, vs, am, acs, 0/*ace*/ ,t);
                c=scurve_acc_dcc(1,vs, ve, am, 0/*acs*/, ace ,t);
            }
            lb=ltot-(a.cs+c.cs);

            b=scurve_lineair(t,vs,lb);
            ct=a.ct+b.ct+c.ct;
        }

        t=at_time;
        if(t<=a.ct){
            r=scurve_acc_dcc(0,vo, vs, am, acs, 0/*ace*/ ,t);
        }
        if(t>a.ct && t<(a.ct+b.ct)){
            t-=a.ct;
            r=scurve_lineair(t,vs,lb);
            r.sr+=a.cs;
        }
        if(t>=a.ct+b.ct && t<=a.ct+b.ct+c.ct){
            t-=a.ct;
            t-=b.ct;
            r=scurve_acc_dcc(1,vs, ve, am, 0/*acs*/, ace ,t);
            r.sr+=a.cs;
            r.sr+=b.cs;


        }
        r.ct=ct;
        // std::cout<<"mode 2"<<std::endl;
        return r;
    }

    //! Normal curve, starting with acc<0
    if(acs<0 && vs>=vo && vs>=ve){
        //! Get curve time.
        t=0;
        RESULT a,b,c,d;
        double ct=0;
        a=scurve_acc_dcc(1,vo, vo-1, am, acs, 0/*ace*/ ,t);
        b=scurve_acc_dcc(0,vo-1, vs, am, 0/*acs*/, 0/*ace*/ ,t);
        d=scurve_acc_dcc(1,vs, ve, am, 0/*acs*/, ace ,t);

        double lc=ltot-(a.cs+b.cs+d.cs);
        c=scurve_lineair(0,vs,lc);
        ct=a.ct+b.ct+c.ct+d.ct;

        /*
        std::cout<<"l0:"<<a.cs<<std::endl;
        std::cout<<"l1:"<<b.cs<<std::endl;
        std::cout<<"l2:"<<c.cs<<std::endl;
        std::cout<<"l3:"<<d.cs<<std::endl;
        std::cout<<"ltot:"<<a.cs+b.cs+c.cs+d.cs<<std::endl;
        std::cout<<"t0:"<<a.ct<<std::endl;
        std::cout<<"t1:"<<b.ct<<std::endl;
        std::cout<<"t2:"<<c.ct<<std::endl;
        std::cout<<"t3:"<<d.ct<<std::endl;
        std::cout<<"ttot:"<<a.ct+b.ct+c.ct+d.ct<<std::endl;*/

        if(lc<0){
            // std::cout<<"lin stage<0!"<<std::endl;
            //! Sample curve to fit ltot.
            while(a.cs+b.cs+d.cs>ltot){
                vs-=0.1;
                a=scurve_acc_dcc(1,vo, vo-1, am, acs, 0/*ace*/ ,t);
                b=scurve_acc_dcc(0,vo-1, vs, am, 0/*acs*/, 0/*ace*/ ,t);
                d=scurve_acc_dcc(1,vs, ve, am, 0/*acs*/, ace ,t);
            }
            lc=ltot-(a.cs+b.cs+d.cs);

            c=scurve_lineair(t,vs,lc);
            ct=a.ct+b.ct+c.ct+d.ct;
        }

        t=at_time;
        if(t<a.ct){
            r=scurve_acc_dcc(1,vo, vo-1, am, acs, 0/*ace*/ ,t);
        }
        if(t>=a.ct && t<(a.ct+b.ct)){
            t-=a.ct;
            r=scurve_acc_dcc(0,vo-1, vs, am, 0/*acs*/, 0/*ace*/ ,t);
            r.sr+=a.cs;
        }
        if(t>=a.ct+b.ct && t<a.ct+b.ct+c.ct){
            t-=a.ct;
            t-=b.ct;
            r=scurve_lineair(t,vs,lc);
            r.sr+=a.cs;
            r.sr+=b.cs;
        }
        if(t>=a.ct+b.ct+c.ct && t<=a.ct+b.ct+c.ct+d.ct){
            t-=a.ct;
            t-=b.ct;
            t-=c.ct;
            r=scurve_acc_dcc(1,vs, ve, am, 0/*acs*/, ace ,t);
            r.sr+=a.cs;
            r.sr+=b.cs;
            r.sr+=c.cs;
        }
        r.ct=ct;
        // std::cout<<"mode 3"<<std::endl;
        return r;
    }

    return r;
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
MainWindow::RESULT MainWindow::scurve_acc_dcc(int sct, double vo, double ve, double am, double acs, double ace, double at_time){

    RESULT r;

    //! ***** = Addons for scurve mirror effect.
    if(sct==1){
        //! ***** mirror acceleration start, end.
        double temp_acs=std::abs(acs);
        acs=std::abs(ace);
        ace=temp_acs;

        //! ***** Mirror ve & vo.
        double temp_ve=ve;
        ve=vo;
        vo=temp_ve;
    }

    //! Minimal maximum acceleration.
    if(am<0.1){
        am=0.1;
        std::cout<<"am set to:"<<am<<std::endl;
    }
    if(acs>(2*am)){
        acs=2*am;
        std::cout<<"acs limited to:"<<acs<<std::endl;
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
    r.cs=stn;                                             //! Curve displacment

    double concave=(t1/2)-ts;
    double convex=te;
    double ct=concave+convex;                                  //! Netto curve time [ct]

    if(sct==1){
        //! ***** Mirror time.
        at_time=ct-at_time;
        if(at_time<0){at_time=0;}
    }

    if(at_time<=ct){
        if(at_time<=concave){
            //! ts is relative startpos if start acc is used.
            t=at_time+ts;
            r.sr=vo*t+jm*(t*t*t)/6;                       //! Displacement.
            r.sr-=vo*ts+jm*(ts*ts*ts)/6;                  //! Substract front acc.
            r.vr=vo+jm*(t*t)/2;                           //! Velocity.
            r.ar=jm*t;                                    //! Acceleration.

            if(sct==1){                                 //! **** Scurve type : dcc.
                r.ar=-std::abs(r.ar);                            //! **** Dcc curve.
                r.sr=stn-r.sr;                              //! **** Dcc curve, mirror displacment.
            }

        }
        if(at_time>concave){
            t=at_time-concave;

            th=t1/2;
            vh=vo+jm*(th*th)/2;                         //! Velocity at start of convex period. Velocity at first inflection point.

            r.sr=vh*t + as*(t*t)/2 - jm*(t*t*t)/6;
            r.sr+=sce;                                    //! Add displacement concave period.
            r.vr=vh + as*t - jm*(t*t)/2;
            r.ar=as-jm*t;

            if(sct==1){                                 //! **** Scurve type : dcc.
                r.ar=-std::abs(r.ar);                            //! ***** Dcc curve.
                r.sr=stn-r.sr;                              //! **** Dcc curve, mirror displacment.
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
        std::cout<<"1.convex+concave period  :"<<r.ct<<std::endl;
    }

    r.ct=ct;
    return r;
}

MainWindow::RESULT MainWindow::scurve_lineair(double at_time, double vs, double cs){
    RESULT r;

    //! Limits.
    if(vs==0){
        std::cout<<"vel can not be 0"<<std::endl;
        return r;
    }

    //! Formula : s=v*t
    r.ct=cs/vs;                 //! Total curve time [ct].
    r.ct=std::abs(r.ct);        //! Used when velocity is negative.

    r.sr=vs*at_time;            //! Displacement [sr] at timestamp [at_time].

    r.cs=vs*r.ct;               //! Total curve displacment [cs].

    r.vr=vs;                    //! Velocity result = Velocity max.

    if(r.ct<0){
        // std::cout<<"lineair curve time <0"<<std::endl;
        // return 1;
    }
    if(r.sr<0){
        // std::cout<<"lineair curve displacment <0"<<std::endl;
        // return 1;
    }

    return r;
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
    OpencascadeWidget->Remove_all();
    on_pushButton_show_acc_dcc_curve_pressed();
}

void MainWindow::on_doubleSpinBox_velocity_end_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_show_acc_dcc_curve_pressed();
}

void MainWindow::on_doubleSpinBox_acceleration_max_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_show_acc_dcc_curve_pressed();
}

void MainWindow::on_doubleSpinBox_acceleration_end_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_show_acc_dcc_curve_pressed();
}

void MainWindow::on_doubleSpinBox_acceleration_begin_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
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
    OpencascadeWidget->Remove_all();
    on_pushButton_show_lin_curve_pressed();
}

void MainWindow::on_doubleSpinBox_lin_displacment_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_show_lin_curve_pressed();
}

void MainWindow::on_doubleSpinBox_mb_velocity_start_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_velocity_end_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_velocity_max_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_acc_max_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_acc_begin_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_acc_end_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_doubleSpinBox_mb_pathlenght_valueChanged(double arg1)
{
    OpencascadeWidget->Remove_all();
    on_pushButton_create_motion_block_pressed();
}

void MainWindow::on_pushButton_show_acc_dcc_curve_pressed()
{

}

void MainWindow::on_pushButton_show_lin_curve_pressed()
{

}
