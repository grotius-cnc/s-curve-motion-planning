#include <iostream>
#include <math.h>

//http://www.et.byu.edu/~ered/ME537/Notes/Ch5.pdf
//Ideal S-Curve

void test();

int main()
{
    test();
    return 0;
}

void test(){

    std::cout << "" << std::endl;
    std::cout << "test ok" << std::endl;

    double T  = 4;      // Total Acc time.
    double t  = T/2;    // A period time, concave or convex.
    double ti = t;      // A period time, used for invert time at deacceleration.
    double As = 2;      // mm/sec2, Maximum acceleration encountered at the S-curve inflection point.
    double Ar = As/2;   // Acceleration as normal lineair transition curve, Note at page 3 of document. As = 2*Ar.
    double A;           // Current acceleration.
    double Jm = 2*As/T; // Max Jerk for profile, 2*As/T <= Jm.
    double V;           // Current velocity.
    double Vo = 0;      // Vel at start of concave period.
    double Vh;          // Vel at start of convex period.
    double Sa;          // S(t) Distance at concave period, changed from S to Sa.
    double Sb;          // S(t) Distance at convex period, changed from S to Sb.
    double St;          // Total distance of acc path.


    //! ----------------- ---- -- Concave period, figur 5.1. -- ---- ----------------------------
    //! up

    for(double t=0; t<=T/2; t+=0.5){
        Sa = Vo*t + Jm*(t*t*t)/6;
        V = Vo + Jm*(t*t)/2;
        A = Jm*t;
        std::cout << "----- Results acc concave period -----"           << std::endl;
        std::cout << "period   t     :" << t                            << std::endl;
        std::cout << "distance S     :" << Sa                           << std::endl;
        std::cout << "velocity V     :" << V                            << std::endl;
        std::cout << "acceleration A :" << A                            << std::endl;
    }

    // Initialize Vh.
    Vh=V;

    //! ----------------- ---- -- Convex period, figur 5.1. -- ---- ----------------------------
    //! down

    for(double t=0; t<=T/2; t+=0.5){
        Sb = Vh*t + As*(t*t)/2 -Jm*(t*t*t)/6;
        V = Vh + As*t -Jm*(t*t)/2;
        A = As - (Jm*t);
        std::cout << ""                                                 << std::endl;
        std::cout << "---- Results acc convex period ----"              << std::endl;
        std::cout << "period   t     :" << t                            << std::endl;
        std::cout << "distance S     :" << Sb                           << std::endl;
        std::cout << "velocity V     :" << V                            << std::endl;
        std::cout << "acceleration A :" << A                            << std::endl;
    }

    //! The DeAcceleration start here !!
    //! Try to mirror the acc into dcc.
    //! We mirror the time t.
    //! We mirror the distance Sa and Sb.
    //!
    //!
    //!
    //! ----------------- ---- -- Convex period, figur 5.1. -- ---- ----------------------------
    //! down

    double temp=Sb;
    for(double t=0; t<=T/2; t+=0.5){
        Sb = temp - ( Vh*(ti-t) + As*((ti-t)*(ti-t))/2 -Jm*((ti-t)*(ti-t)*(ti-t))/6 );
        V = Vh + As*(ti-t) -Jm*((ti-t)*(ti-t))/2;
        A = -abs(As - (Jm*(ti-t)));
        std::cout << ""                                                 << std::endl;
        std::cout << "---- Results dcc convex period ----"              << std::endl;
        std::cout << "period   t     :" << t                            << std::endl;
        std::cout << "distance S     :" << Sb                           << std::endl;
        std::cout << "velocity V     :" << V                            << std::endl;
        std::cout << "acceleration A :" << A                            << std::endl;
    }

    //! ----------------- ---- -- Concave period, figur 5.1. -- ---- ----------------------------
    //! up

    temp = Sa;
    for(double t=0; t<=T/2; t+=0.5){
        Sa = temp - Vo*(ti-t) + Jm*((ti-t)*(ti-t)*(ti-t))/6;
        V = Vo + Jm*((ti-t)*(ti-t))/2;
        A = -abs(Jm*(ti-t));
        std::cout << "----- Results dcc concave period -----"           << std::endl;
        std::cout << "period   t     :" << t                            << std::endl;
        std::cout << "distance S     :" << Sa                           << std::endl;
        std::cout << "velocity V     :" << V                            << std::endl;
        std::cout << "acceleration A :" << A                            << std::endl;
    }
}










