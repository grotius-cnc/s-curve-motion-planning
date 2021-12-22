#include <stdio.h>
#include "scurve.h"

//! Install scurve library:
//! sudo make install

//! Compile slogan main.c:
//! g++ example_1.c -o example_1 -L$PWD -lscurve

//! Run:
//! ./example_1

//! Inputs:
//! vs=velocity max.
//! am=acceleration max.
//! ltot=pathlenght.
//! vo=velocity begin.
//! acs=acceleration begin.
//! ve=velocity end.
//! ace=acceleration end.
//! at_time=curve request at time [t]
//! Results:
//! sr=current displacement.
//! vr=current velocity.
//! ar=current acceleration.
//! ct=total curve time.

int main(int argc, char *argv[]) {
		
	double vs=10; 
	double am=2;
	double vo=0;
	double acs=0;
	double ltot=100;
	double ve=0;
	double ace=0;
	double at_time=0; 
	double sr=0;
	double vr=0; 
	double ar=0;
	double ct=0;
	
	int r=0;
	//! Request total motion time [ct]
	r+=Scurve().motion_block( 	vs, 
								am, 
								vo, 
								acs, 
								ltot, 
								ve, 
								ace, 
								at_time, 
								sr,
								vr, 
								ar,
								ct);
	if(!r){									
		printf("traject time: %f \n", ct);
	} else {printf("error \n");}
	
	//! For the total motion time [ct], request a state at time stamp [t]
	for(double t=0; t<ct; t+=0.1){
		r+=Scurve().motion_block(	vs, 
									am, 
									vo, 
									acs, 
									ltot, 
									ve, 
									ace, 
									t, 
									sr,
									vr, 
									ar,
									ct);
		if(!r){							
			printf("current displacment : %f \n", sr);
			printf("current velocity    : %f \n", vr);
			printf("current acceleration: %f \n", ar);	
		} else {printf("error \n");}
	}													
}

