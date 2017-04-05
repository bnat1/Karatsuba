#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <typeinfo>
#include <time.h>
#include "posint.h"
using namespace std;

int main() {
  
  int base = 2;
  int pow = 1;
	PosInt::setBase(base, pow);
	srand(time(NULL));
  // 2148 ^ 2
  // PosInt test2a(7369401);
  // PosInt test2a2(7369401);
  // PosInt test2b(2741495);
  // test2a.fastMul(test2b);
  // test2a2.mul(test2b);

  // // test2a.print_array(cout);
  // test2a2.print(cout);
  // cout << endl;
  // test2a.print(cout);
  
 

  // Set base to 16^4
  // This causes numbers to display in hex.  
  // However, base must be no greater than 2^15 when
  // using 32-bit integers, so we can only fit 3 hex 
  // digits (or 12 bits) per 32-bit int.  It is more
  // space-efficient to use the default, but then the
  // numbers will print in binary.
  // PosInt::setBase(16, 3);

	// timer
	clock_t startTime, stopTime, mulTimePassed, fastMulTimePassed;

  int trialsPerDigit = 10;
  int maxDigits = 6000;
  float totalMulTime;
  float totalFastMulTime;
  float avgTimePerMul;
  float avgTimePerFastMul;  
  bool crossOver;
  cout << "base: " << base << " ^ " << pow << endl;
  cout << "timing unit: 1 / " << CLOCKS_PER_SEC << " seconds" << endl;
  cout << "maxDigits: " << maxDigits << endl;
  cout << "trialsPerDigit: " << trialsPerDigit << endl;

  PosInt one(1);
  PosInt two(2);
  PosInt four(4);
  PosInt five(5);
  PosInt basePosInt(base);

  PosInt baseToFive(basePosInt);
  baseToFive.pow(five);
  PosInt baseToFour(basePosInt);
  baseToFour.pow(four);
  
  PosInt lowerBound(one);
  PosInt upperBound(basePosInt);
  PosInt upperLowerDiff(upperBound);
  upperLowerDiff.sub(lowerBound);

	PosInt mulTester;
	PosInt fastMulTester;
	PosInt x;
	PosInt y;

	cout << "digits\t" << "average mul() time\t" << "average fastMul() time\t" << "crossover achieved?\t" << endl; 
  for(int i = 1; i <= maxDigits; i += 5){
    totalMulTime = 0;
    totalFastMulTime = 0;
		for(int j = 0; j < trialsPerDigit; ++j){
			x.rand(upperLowerDiff);
			x.add(lowerBound);
			y.rand(upperLowerDiff);
			y.add(lowerBound);
      // cout << "digits: " << i << endl;
      // cout << "x: " << x << endl;
      // cout << "y: " << y <<endl;
			mulTester.set(x);
			fastMulTester.set(x);

      //start timer
      startTime = clock();
      mulTester.mul(y);
      //stop timer
      stopTime = clock();
      mulTimePassed = stopTime - startTime;
      totalMulTime += mulTimePassed;
			//start timer
      startTime = clock();
			fastMulTester.fastMul(y);
			//stop timer
			stopTime = clock();
      fastMulTimePassed = stopTime - startTime;      
      totalFastMulTime += fastMulTimePassed;			
		}
		avgTimePerMul = totalMulTime / trialsPerDigit;
		avgTimePerFastMul = totalFastMulTime / trialsPerDigit;
		crossOver = avgTimePerFastMul < avgTimePerMul;

		cout << i << '\t'; printf("%.0f\t", avgTimePerMul); printf("%.0f\t", avgTimePerFastMul);  cout << crossOver << endl; 	
    if(i == 1) {
      --i;
      upperBound.set(1);
    }

    lowerBound.set(upperBound);
    lowerBound.mul(baseToFour);
    upperBound.mul(baseToFive);
    upperLowerDiff.set(upperBound);
    upperLowerDiff.sub(lowerBound);
  }
}