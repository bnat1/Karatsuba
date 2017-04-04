#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <typeinfo>
#include <sys/time.h>
#include <time.h>
#include "posint.h"
using namespace std;

int main() {
  
	PosInt::setBase(10, 1);
	srand(time(NULL));
 //  //2148 ^ 2
 //  PosInt test2a(1234);
 //  PosInt test2b(54390801);
 //  PosInt test2a2(1234);
 //  test2a.fastMul(test2b);
 //  test2a2.mul(test2b);
 //  // test2a.print_array(cout);
 //  test2a2.print(cout);
 //  cout << endl;
 //  test2a.print(cout);
  

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

  int trialsPerDigit = 5;
  int maxDigits = 50000;
  float totalMulTime;
  float totalFastMulTime;
  float avgTimePerMul;
  float avgTimePerFastMul;  
  bool crossOver;
  cout << "timing unit: 1 / " << CLOCKS_PER_SEC << " of a second" << endl;
  cout << "maxDigits: " << maxDigits << endl;
  cout << "trialsPerDigit: " << trialsPerDigit << endl;

	PosInt lowerBound(1);
	PosInt upperBound(10);
	PosInt upperLowerDiff(upperBound);
  PosInt ten(10);
  PosInt oneHundred(100);
  PosInt googol(ten);
  googol.pow(oneHundred);
  PosInt nine(9);
  PosInt ninetyNine(99);
  PosInt tenTo99th(ten);
  tenTo99th.pow(ninetyNine); 
	upperLowerDiff.sub(lowerBound);

	PosInt mulTester;
	PosInt fastMulTester;
	PosInt x;
	PosInt y;

	cout << "digits\t" << "average mul() time\t" << "average fastMul() time\t" << "crossover achieved?\t" << endl; 
  for(int i = 1; i < maxDigits; i += 100){
    totalMulTime = 0;
    totalFastMulTime = 0;
		for(int j = 0; j < trialsPerDigit; ++j){
			x.rand(upperLowerDiff);
			x.add(lowerBound);
			y.rand(upperLowerDiff);
			y.add(lowerBound);
			mulTester.set(x);
			//start timer
      startTime = clock();
			mulTester.mul(y);
			//stop timer
      stopTime = clock();
			mulTimePassed = stopTime - startTime;
			totalMulTime += mulTimePassed;


			fastMulTester.set(y);
			//start timer
      startTime = clock();
			fastMulTester.mul(y);
			//stop timer
			stopTime = clock();
      fastMulTimePassed = stopTime - startTime;      
      totalFastMulTime += fastMulTimePassed;			
		}
		avgTimePerMul = totalMulTime / trialsPerDigit;
		avgTimePerFastMul = totalFastMulTime / trialsPerDigit;
		crossOver = avgTimePerFastMul < avgTimePerMul;

		cout << i << '\t'; printf("%.0f\t", avgTimePerMul); printf("%.0f\t", avgTimePerFastMul);  cout << crossOver << endl; 	

		lowerBound.set(upperBound);
    lowerBound.mul(tenTo99th);
		upperBound.mul(googol);
		upperLowerDiff.set(upperBound);
		upperLowerDiff.sub(lowerBound);
  if (i == 1){ i -= 1;}
  }
}