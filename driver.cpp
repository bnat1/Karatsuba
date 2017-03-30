#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <typeinfo>
#include <sys/time.h>
#include "posint.h"
using namespace std;

int main() {

  // Set base to 16^4
  // This causes numbers to display in hex.  
  // However, base must be no greater than 2^15 when
  // using 32-bit integers, so we can only fit 3 hex 
  // digits (or 12 bits) per 32-bit int.  It is more
  // space-efficient to use the default, but then the
  // numbers will print in binary.
  // PosInt::setBase(16, 3);
	PosInt::setBase(10, 1);
	srand(time(NULL));

	cout.precision(2);
	//timer
	timespec startTime, stopTime, mulTimePassed, fastMulTimePassed;

	int trialsPerDigit = 2;
	int maxDigits = 20;
	int totalMulTime;
	int totalFastMulTime;
	float avgTimePerDigitMul;
	float avgTimePerDigitFastMul;	
	bool crossOver;
	PosInt ten(10);
	PosInt lowerBound(1);
	PosInt upperBound(10);
	PosInt upperLowerDiff(upperBound);
	upperLowerDiff.sub(lowerBound);

	PosInt mulTester;
	PosInt fastMulTester;
	PosInt x;
	PosInt y;
	for(int i = 1; i < maxDigits; ++i){
		totalMulTime = 0;
		totalFastMulTime = 0;
		cout << "Trial #\t" << "# Digits\t" << "x\t" << "y\t" << "mul time (ns)\t" << "fastMul time (ns)\t" << endl; 
		for(int j = 0; j < trialsPerDigit; ++j){
			x.rand(upperLowerDiff);
			x.add(lowerBound);
			y.rand(upperLowerDiff);
			y.add(lowerBound);
			mulTester.set(x);
			//start timer
			clock_gettime(CLOCK_REALTIME, &startTime);
			mulTester.mul(y);
			//stop timer
			clock_gettime(CLOCK_REALTIME, &stopTime);
			mulTimePassed.tv_nsec = stopTime.tv_nsec - startTime.tv_nsec;
			totalMulTime += mulTimePassed.tv_nsec;


			fastMulTester.set(y);
			//start timer
			clock_gettime(CLOCK_REALTIME, &startTime);
			fastMulTester.mul(y);
			//stop timer
			clock_gettime(CLOCK_REALTIME, &stopTime);
			fastMulTimePassed.tv_nsec = stopTime.tv_nsec - startTime.tv_nsec;			
			totalFastMulTime += fastMulTimePassed.tv_nsec;
			
			cout << j+1 << '\t' << i << '\t' << x << '\t' << y << '\t' << mulTimePassed.tv_nsec << '\t' << fastMulTimePassed.tv_nsec << '\t' << endl;			
		}

		avgTimePerDigitMul = (float)totalMulTime / (float)i;
		avgTimePerDigitFastMul = (float)totalFastMulTime / (float)i;
		crossOver = avgTimePerDigitFastMul < avgTimePerDigitMul;

		cout << "# Trials\t" << "# Digits\t" << "avg ns/mul\t" << "avg ns/fastMul\t" << "crossover\t" << endl; 

		cout << trialsPerDigit << '\t' << i << '\t'; printf("%g\t", avgTimePerDigitMul); printf("%g\t", avgTimePerDigitFastMul);  cout << '\t' << crossOver << endl << endl; 	

		lowerBound.set(upperBound);
		upperBound.mul(ten);
		upperLowerDiff.set(upperBound);
		upperLowerDiff.sub(lowerBound);
	}

}
  
// want to output:

//  | trial # | # digits | x	| y	| mul time (ns) | fastMul time (ns) | 
//	|# trials | # digits | avg mul time |avg fast mul time | overall faster