
#include <stdio.h>
#include "scurve.h"


//! Install scurve library:
//! sudo make install

//! Compile slogan:
//! g++ main.c -o main -L$PWD -lscurve

//! Run:
//! ./main

int main(int argc, char *argv[]) {
	printf("hello\n");

	Scurve().example_lineair();
	Scurve().example_scurve();
	Scurve().example_motion();
}
