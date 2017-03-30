#include <iostream>
#include <stdlib.h>
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

	int trialsPerBit = 2;
	int maxBits = 64;

	PosInt one(1);
	PosInt two(2);
	PosInt lowerBound(1);
	PosInt upperBound(2);
	PosInt upperLowerDiff(upperBound);
	upperLowerDiff.sub(lowerBound);

	PosInt mulTester;
	PosInt fastMulTester;
	PosInt x;
	PosInt y;
	for(int i = 1; i < maxBits; ++i){
		cout << "bits: " << i << endl;
		cout << "lower bound: " << lowerBound << endl;
		cout << "upper bound: " << upperBound << endl;			
		for(int j = 0; j < trialsPerBit; ++j){
			x.rand(upperLowerDiff);
			x.add(lowerBound);
			y.rand(upperLowerDiff);
			y.add(lowerBound);
			cout << "x: " << x << endl;
			cout << "y: " << y << endl;
		}
		lowerBound.set(upperBound);
		upperBound.mul(two);
		upperLowerDiff.set(upperBound);
		upperLowerDiff.sub(lowerBound);
	}

}
  
