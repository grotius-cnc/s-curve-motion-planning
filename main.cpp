#include <iostream>
#include <math.h>

int main()
{
    //http://www.et.byu.edu/~ered/ME537/Notes/Ch5.pdf
    //Ideal S-Curve

    double T = 4;       // Total Acc time.
    double t=T/2;       // A period time.
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
    //!

    Sa = Vo*t + Jm*(t*t*t)/6;

    V = Vo + Jm*(t*t)/2;

    A = Jm*t;

    std::cout << "----- Results concave period -----"               << std::endl;
    std::cout << "period   t     :" << t                            << std::endl;
    std::cout << "distance S     :" << Sa                           << std::endl;
    std::cout << "velocity V     :" << V                            << std::endl;
    std::cout << "acceleration A :" << A                            << std::endl;

    // Initialize Vh.
    Vh=V;

    //! ----------------- ---- -- Convex period, figur 5.1. -- ---- ----------------------------
    //!

    Sb = Vh*t + As*(t*t)/2 -Jm*(t*t*t)/6;

    V = Vh + As*t -Jm*(t*t)/2;

    A = As - (Jm*t);

    std::cout << ""                                                 << std::endl;
    std::cout << "---- Results convex period ----"                  << std::endl;
    std::cout << "period   t     :" << t                            << std::endl;
    std::cout << "distance S     :" << Sb                           << std::endl;
    std::cout << "velocity V     :" << V                            << std::endl;
    std::cout << "acceleration A :" << A                            << std::endl;

    std::cout << " "                                                << std::endl;
    std::cout << "---- Results S curve acceleration ----"           << std::endl;
    std::cout << "Acceleration Time            : " << 2*t           << std::endl;
    std::cout << "Concave + convex distance St : " << Sa+Sb         << std::endl;


    //! ----------------- ---- -- Control calculation as lineair curve  -- ---- ----------------------------
    //!

    // Normal acceleraton time T
    std::cout << ""<<std::endl;
    std::cout << "---- Results normal lineair acceleration ----"    << std::endl;
    std::cout << "Check Acceleration time T     : " << T            << std::endl;

    // Normal distance formula : S=Ut+0.5 *A*t^2
    St = (0*T) + (0.5*Ar) * (T*T);
    std::cout << "Check acceleration distance S : "<< St            << std::endl;

    // Normal velocity formula t=v/a
    V = T * Ar;
    std::cout << "Check velocity V              : "<< V             << std::endl;

    // Normal acceleration A, Ar=A2/2, see comments above how to find Ar.
    std::cout << "Check acceleration A          : "<< Ar            << std::endl;

    return 0;
}










