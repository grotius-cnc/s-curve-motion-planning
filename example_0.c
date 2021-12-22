#include <stdio.h>
#include "scurve.h"

//! Install scurve library:
//! sudo make install

//! Compile slogan main.c:
//! g++ example_0.c -o example_0 -L$PWD -lscurve

//! Run:
//! ./example_0

int main(int argc, char *argv[]) {
	Scurve().example_motion();
}

//! Output ./main:
//! at_time:0 sr:0.0576 vr:0.288 ar:0.96 ct:15
//! Time taken by function nanoseconds: 7473 milliseconds:0.00747
