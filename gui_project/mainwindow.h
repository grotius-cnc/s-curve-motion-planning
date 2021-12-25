#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#ifdef Success
#undef Success
#endif

// libocc
#include <opencascade.h>
using namespace occ;

#include <draw_primitives.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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

    RESULT scurve_lineair(double at_time, double vs, double cs);
    RESULT scurve_acc_dcc(int sct, double vo, double ve, double am, double acs, double ace, double at_time);
    RESULT motion(double vs, double am, double vo, double acs, double ltot, double ve, double ace, double at_time);

    struct CONTROL{
        //! Maximum velocity
        double vs=0;
        //! Velocity overide 0-100%
        double vso=0;
        //! Maximum acceleration
        double am=0;
        //! Interval time.
        double i_time=0;
        //! Traject time.
        double tr_time=0;
        //! Traject displacment.
        double tr_s=0;
    };

    struct PATH{
        //! Maximum velocity.
        double vs=0;
        //! Velocity begin.
        double vo=0;
        //! Acceleration start.
        double acs=0;
        //! Path lenght.
        double ltot=0;
        //! Velocity end.
        double ve=0;
        //! Acceleration end.
        double ace=0;
        //! Distance to go.
        double dtg=0;
    };

    void simulate();
    void runner(CONTROL c, std::vector<PATH> pathvec);

private slots:

    void on_pushButton_show_acc_dcc_curve_pressed();

    void on_doubleSpinBox_velocity_start_valueChanged(double arg1);

    void on_doubleSpinBox_velocity_end_valueChanged(double arg1);

    void on_doubleSpinBox_acceleration_max_valueChanged(double arg1);

    void on_doubleSpinBox_acceleration_begin_valueChanged(double arg1);

    void on_comboBox_scruve_type_currentIndexChanged(int index);

    void on_doubleSpinBox_lin_velocity_valueChanged(double arg1);

    void on_doubleSpinBox_lin_displacment_valueChanged(double arg1);

    void on_pushButton_show_lin_curve_pressed();

    void on_pushButton_clear_lin_curve_pressed();

    void on_pushButton_create_motion_block_pressed();

    void on_pushButton_clear_dcc_curve_pressed();

    void on_doubleSpinBox_mb_velocity_start_valueChanged(double arg1);

    void on_doubleSpinBox_mb_velocity_end_valueChanged(double arg1);

    void on_doubleSpinBox_mb_velocity_max_valueChanged(double arg1);

    void on_doubleSpinBox_mb_acc_max_valueChanged(double arg1);

    void on_doubleSpinBox_mb_acc_begin_valueChanged(double arg1);

    void on_doubleSpinBox_mb_acc_end_valueChanged(double arg1);

    void on_pushButton_mb_clear_pressed();

    void on_doubleSpinBox_mb_pathlenght_valueChanged(double arg1);

    void on_doubleSpinBox_acceleration_end_valueChanged(double arg1);

    void on_pushButton_simulate_path_pressed();

private:
    Ui::MainWindow *ui;
    Opencascade *OpencascadeWidget;

};
#endif // MAINWINDOW_H

