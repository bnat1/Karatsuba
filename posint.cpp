#include <cctype>
#include <cstdlib>
#include <string>
#include <sstream>
#include "posint.h"
using namespace std;

/******************** BASE ********************/

int PosInt::B = 0x8000;
int PosInt::Bbase = 2;
int PosInt::Bpow = 15;

void PosInt::setBase(int base, int pow) {
  Bbase = base;
  Bpow = pow;
  B = base;
  while (pow > 1) {
    B *= Bbase;
    --pow;
  }
}

/******************** I/O ********************/

void PosInt::read (const char* s) {
  string str(s);
  istringstream sin (str);
  read(sin);
}

void PosInt::set(int x) {
  digits.clear();

  if (x < 0)
    throw MPError("Can't set PosInt to negative value");

  while (x > 0) {
    digits.push_back(x % B);
    x /= B;
  }
}

void PosInt::set (const PosInt& rhs) {
  if (this != &rhs)
    digits.assign (rhs.digits.begin(), rhs.digits.end());
}

void PosInt::print_array(ostream& out) const {
  out << "[ls";
  for (int i=0; i<digits.size(); ++i)
    out << ' ' << digits[i];
  out << " ms]";
}

void PosInt::print(ostream& out) const {
  if (digits.empty()) out << 0;
  else {
    int i = digits.size()-1;
    int pow = B/Bbase;
    int digit = digits[i];
    for (; digit < pow; pow /= Bbase);
    while (true) {
      for (; pow>0; pow /= Bbase) {
        int subdigit = digit / pow;
        if (subdigit < 10) out << subdigit;
        else out << (char)('A' + (subdigit - 10));
        digit -= subdigit*pow;
      }
      if (--i < 0) break;
      digit = digits[i];
      pow = B/Bbase;
    }
  }
}

void PosInt::read (istream& in) {
  vector<int> digstack;
  while (isspace(in.peek())) in.get();
  int pow = B/Bbase;
  int digit = 0;
  int subdigit;
  while (true) {
    int next = in.peek();
    if (isdigit(next)) subdigit = next-'0';
    else if (islower(next)) subdigit = next - 'a' + 10;
    else if (isupper(next)) subdigit = next - 'A' + 10;
    else subdigit = Bbase;
    if (subdigit >= Bbase) break;
    digit += pow*subdigit;
    in.get();
    if (pow == 1) {
      digstack.push_back (digit);
      pow = B/Bbase;
      digit = 0;
    }
    else pow /= Bbase;
  }
  pow *= Bbase;
  if (pow == B && !digstack.empty()) {
    pow = 1;
    digit = digstack.back();
    digstack.pop_back();
  }
  int pmul = B/pow;
  digits.assign (1, digit/pow);
  for (int i=digstack.size()-1; i >= 0; --i) {
    digits.back() += (digstack[i] % pow) * pmul;
    digits.push_back (digstack[i] / pow);
  }
  normalize();
}

int PosInt::convert () const {
  int val = 0;
  int pow = 1;
  for (int i = 0; i < digits.size(); ++i) {
    val += pow * digits[i];
    pow *= B;
  }
  return val;
}

ostream& operator<< (ostream& out, const PosInt& x) { 
  x.print(out); 
  return out;
}

istream& operator>> (istream& in, PosInt& x) { 
  x.read(in);
  return in;
}

/******************** RANDOM NUMBERS ********************/

// Produces a random number between 0 and n-1
static int randomInt (int n) {
  int max = RAND_MAX - ((RAND_MAX-n+1) % n);
  int r;
  do { r = rand(); }
  while (r > max);
  return r % n;
}

// Sets this PosInt to a random number between 0 and x-1
void PosInt::rand (const PosInt& x) {
  if (this == &x) {
    PosInt xcopy(x);
    rand(xcopy);
  }
  else {
    PosInt max;
    max.digits.assign (x.digits.size(), 0);
    max.digits.push_back(1);
    PosInt rem (max);
    rem.mod(x);
    max.sub(rem);
    do {
      digits.resize(x.digits.size());
      for (int i=0; i<digits.size(); ++i)
        digits[i] = randomInt(B);
      normalize();
    } while (compare(max) >= 0);
    mod(x);
  }
}

/******************** UTILITY ********************/

// Removes leading 0 digits
void PosInt::normalize () {
  int i;
  for (i = digits.size()-1; i >= 0 && digits[i] == 0; --i);
  if (i+1 < digits.size()) digits.resize(i+1);
}

bool PosInt::isEven() const {
  if (B % 2 == 0) return digits.empty() || (digits[0] % 2 == 0);
  int sum = 0;
  for (int i = 0; i < digits.size(); ++i)
    sum += digits[i] % 2;
  return sum % 2 == 0;
}

// Result is -1, 0, or 1 if a is <, =, or > than b,
// up to the specified length.
int PosInt::compareDigits (const int* a, int alen, const int* b, int blen) {
  int i = max(alen, blen)-1;
  for (; i >= blen; --i) {
    if (a[i] > 0) return 1;
  }
  for (; i >= alen; --i) {
    if (b[i] > 0) return -1;
  }
  for (; i >= 0; --i) {
    if (a[i] < b[i]) return -1;
    else if (a[i] > b[i]) return 1;
  }
  return 0;
}

// Result is -1, 0, or 1 if this is <, =, or > than rhs.
int PosInt::compare (const PosInt& x) const {
  if (digits.size() < x.digits.size()) return -1;
  else if (digits.size() > x.digits.size()) return 1;
  else if (digits.size() == 0) return 0;
  else return compareDigits
    (&digits[0], digits.size(), &x.digits[0], x.digits.size());
}

/******************** ADDITION ********************/

// Computes dest += x, digit-wise
// REQUIREMENT: dest has enough space to hold the complete sum.
void PosInt::addArray (int* dest, const int* x, int len) {
  int i;
  for (i=0; i < len; ++i)
    dest[i] += x[i];

  for (i=0; i+1 < len; ++i) {
    if (dest[i] >= B) {
      dest[i] -= B;
      ++dest[i+1];
    }
  }

  for ( ; dest[i] >= B; ++i) {
    dest[i] -= B;
    ++dest[i+1];
  }
}

// this = this + x
void PosInt::add (const PosInt& x) {
  digits.resize(max(digits.size(), x.digits.size())+1, 0);
  addArray (&digits[0], &x.digits[0], x.digits.size());
  normalize();
}

/******************** SUBTRACTION ********************/

// Computes dest -= x, digit-wise
// REQUIREMENT: dest >= x, so the difference is non-negative
void PosInt::subArray (int* dest, const int* x, int len) {
  int i = 0;
  for ( ; i < len; ++i)
    dest[i] -= x[i];

  for (i=0; i+1 < len; ++i) {
    if (dest[i] < 0) {
      dest[i] += B;
      --dest[i+1];
    }
  }

  for ( ; dest[i] < 0; ++i) {
    dest[i] += B;
    --dest[i+1];
  }
}

// this = this - x
void PosInt::sub (const PosInt& x) {
  if (compare(x) < 0)
    throw MPError("Subtraction would result in negative number");
  else if (x.digits.size() > 0) {
    subArray (&digits[0], &x.digits[0], x.digits.size());
    normalize();
  }
}

/******************** MULTIPLICATION ********************/

// Computes dest = x * y, digit-wise.
// x has length xlen and y has length ylen.
// dest must have size (xlen+ylen) to store the result.
// Uses standard O(n^2)-time multiplication.
void PosInt::mulArray 
  (int* dest, const int* x, int xlen, const int* y, int ylen) 
{
  for (int i=0; i<xlen+ylen; ++i) dest[i] = 0;
  for (int i=0; i<xlen; ++i) {
    for (int j=0; j<ylen; ++j) {
      dest[i+j] += x[i] * y[j];
      dest[i+j+1] += dest[i+j] / B;
      dest[i+j] %= B;
    }
  }
}
void debugArray(const int *x, int len){
  for(int i = 0; i < len; i ++){
    cout << x[i] << " ";
  }
  cout << endl;
}

// Computes dest = x * y, digit-wise, using Karatsuba's method.
// x and y have the same length (len)
// dest must have size (2*len) to store the result.
void PosInt::fastMulArray (int* dest, const int* x, const int* y, int len) {
  // base case
  if(len == 1) {
    mulArray(dest, x, len, y, len); 
    return; 
  }

  // helpful constants
  int lenOver2 = len / 2;
  int twoLenOver2 = 2 * lenOver2;
  int highDigitLen = lenOver2 + len % 2;
  int z2Len = 2 * highDigitLen;
  int digitSumLen = highDigitLen + 1;
  int z1Len = 2 * digitSumLen;

  // pointers to subarrays of inputs
  const int *xHigh = x + lenOver2;
  const int *yHigh = y + lenOver2;
  
  // stores results of recursive fastMul calls
  int *z0 = new int[twoLenOver2]();
  int *z1 = new int[z1Len](); 
  int *z2 = new int[z2Len](); 

  // xDigitSum = xLow + xHigh
  // yDigitSum = yLow + yHigh;
  int *xDigitSum = new int[digitSumLen];
  int *yDigitSum = new int[digitSumLen];
  // zero out extra digit before adding
  xDigitSum[digitSumLen - 1] = 0;
  yDigitSum[digitSumLen - 1] = 0;
  // copy xHigh, yHigh for sum
  for(int i = 0; i < highDigitLen; ++i){ 
    xDigitSum[i] = xHigh[i];
    yDigitSum[i] = yHigh[i];
  }
  addArray(xDigitSum, x, lenOver2);
  addArray(yDigitSum, y, lenOver2);
  // normalize sum before fastMul
  for(;xDigitSum[digitSumLen - 1] == 0 && yDigitSum[digitSumLen - 1] == 0 && digitSumLen > 1; --digitSumLen){}

  // 3 recursive calls to fastMulArray: xLow*yLow, xDigitSum*yDigitSum, xHigh*yHigh
  fastMulArray(z0, x, y, lenOver2);
  fastMulArray(z1, xDigitSum, yDigitSum, digitSumLen);
  delete [] xDigitSum; 
  delete [] yDigitSum;
  fastMulArray(z2, xHigh, yHigh, highDigitLen);
  xHigh = NULL; 
  yHigh = NULL;

  // z1 = z1 - z2 - z0
  subArray(z1, z2, z2Len);
  subArray(z1, z0, twoLenOver2);

  //normalize z1 before adding to dest
  for(;z1[z1Len - 1] == 0 && z1Len > 1; --z1Len){}
  //copy z1 to dest
  for(int i = 0; i < z1Len; ++i){ dest[i+lenOver2] = z1[i]; }
  delete [] z1;
  addArray(dest + twoLenOver2, z2, z2Len);   
  delete [] z2; 
  addArray(dest, z0, twoLenOver2);
  delete [] z0; 

  return;
}

// this = this * x
void PosInt::mul(const PosInt& x) {
  if (this == &x) {
    PosInt xcopy(x);
    mul(xcopy);
    return;
  }

  int mylen = digits.size();
  int xlen = x.digits.size();
  if (mylen == 0 || xlen == 0) {
    set(0);
    return;
  }

  int* mycopy = new int[mylen];
  for (int i=0; i<mylen; ++i) mycopy[i] = digits[i];
  digits.resize(mylen + xlen);
  mulArray(&digits[0], mycopy, mylen, &x.digits[0], xlen);

  normalize();
  delete [] mycopy;
}

// this = this * x, using Karatsuba's method
void PosInt::fastMul(const PosInt& x) {

  // make copy if multiplying this with self
  if(this == &x) {
    PosInt xCopy(x);
    fastMul(xCopy);
    return;
  }

  // if an input has no digits
  int myLen = digits.size();
  int xLen = x.digits.size();
  if(myLen == 0 || xLen == 0) {
    set(0);
    return;
  }

  //least significant digits will be on the left
  int inputLen = max(myLen, xLen);
  int *myCopy = new int[inputLen];
  int *xCopy = new int[inputLen];

  //create zero-padded input arrays
  for (int i = 0; i < myLen; ++i) myCopy[i] = digits[i];
  for (int i = myLen; i < inputLen; ++i) {myCopy[i] = 0;}
  for (int i = 0; i < xLen; ++i) xCopy[i] = x.digits[i];    
  for (int i = xLen; i < inputLen; ++i) {xCopy[i] = 0;}

  //prepare digits for result
  digits.clear();
  digits.resize(inputLen*2);

  //call my fastMulArray function
  fastMulArray(&digits[0], myCopy, xCopy, inputLen);

  normalize();
  delete [] myCopy;
  delete [] xCopy;
}

/******************** DIVISION ********************/

// Computes dest = dest * d, digit-wise
// REQUIREMENT: dest has enough space to hold any overflow.
void PosInt::mulDigit (int* dest, int d, int len) {
  int i;
  for (i=0; i<len; ++i)
    dest[i] *= d;
  for (i=0; i+1<len; ++i) {
    dest[i+1] += dest[i] / B;
    dest[i] %= B;
  }
  for (; dest[i] >= B; ++i) {
    dest[i+1] += dest[i] / B;
    dest[i] %= B;
  }
}

// Computes dest = dest / d, digit-wise, and returns dest % d
int PosInt::divDigit (int* dest, int d, int len) {
  int r = 0;
  for (int i = len-1; i >= 0; --i) {
    dest[i] += B*r;
    r = dest[i] % d;
    dest[i] /= d;
  }
  return r;
}

// Computes division with remainder, digit-wise.
// REQUIREMENTS: 
//   - length of q is at least xlen-ylen+1
//   - length of r is at least xlen
//   - q and r are distinct from all other arrays
//   - most significant digit of divisor (y) is at least B/2
void PosInt::divremArray 
  (int* q, int* r, const int* x, int xlen, const int* y, int ylen)
{
  // Copy x into r
  for (int i=0; i<xlen; ++i) r[i] = x[i];

  // Create temporary array to hold a digit-multiple of y
  int* temp = new int[ylen+1];

  int qind = xlen - ylen;
  int rind = xlen - 1;

  q[qind] = 0;
  while (true) {
    // Do "correction" if the quotient digit is off by a few
    while (compareDigits (y, ylen, r + qind, xlen-qind) <= 0) {
      subArray (r + qind, y, ylen);
      ++q[qind];
    }

    // Test if we're done, otherwise move to the next digit
    if (qind == 0) break;
    --qind;
    --rind;

    // (Under)-estimate the next digit, and subtract out the multiple.
    int quoest = (r[rind] + B*r[rind+1]) / y[ylen-1] - 2;
    if (quoest <= 0) q[qind] = 0;
    else {
      q[qind] = quoest;
      for (int i=0; i<ylen; ++i) temp[i] = y[i];
      temp[ylen] = 0;
      mulDigit (temp, quoest, ylen+1);
      subArray (r+qind, temp, ylen+1);
    }
  }

  delete [] temp;
}

// Computes division with remainder. After the call, we have
// x = q*y + r, and 0 <= r < y.
void PosInt::divrem (PosInt& q, PosInt& r, const PosInt& x, const PosInt& y) {
  if (y.digits.empty()) throw MPError("Divide by zero");
  else if (&q == &r) throw MPError("Quotient and remainder can't be the same");
  else if (x.compare(y) < 0) {
    r.set(x);
    q.set(0);
    return;
  }
  else if (y.digits.size() == 1) {
    int divdig = y.digits[0];
    q.set(x);
    r.set (divDigit (&q.digits[0], divdig, q.digits.size()));
  }
  else if (2*y.digits.back() < B) {
    int ylen = y.digits.size();
    int fac = 1;
    int* scaley = new int[ylen];
    for (int i=0; i<ylen; ++i) scaley[i] = y.digits[i];
    do {
      mulDigit (scaley, 2, ylen);
      fac *= 2;
    } while (2*scaley[ylen-1] < B);

    int xlen = x.digits.size()+1;
    int* scalex = new int[xlen];
    for (int i=0; i<xlen-1; ++i) scalex[i] = x.digits[i];
    scalex[xlen-1] = 0;
    mulDigit (scalex, fac, xlen);
    q.digits.resize(xlen - ylen + 1);
    r.digits.resize(xlen);
    divremArray (&q.digits[0], &r.digits[0], scalex, xlen, scaley, ylen);
    divDigit (&r.digits[0], fac, xlen);
    delete [] scaley;
    delete [] scalex;
  }
  else {
    int xlen = x.digits.size();
    int ylen = y.digits.size();
    int* xarr = NULL;
    int* yarr = NULL;
    if (&x == &q || &x == &r) {
      xarr = new int[xlen];
      for (int i=0; i<xlen; ++i) xarr[i] = x.digits[i];
    }
    if (&y == &q || &y == &r) {
      yarr = new int[ylen];
      for (int i=0; i<ylen; ++i) yarr[i] = y.digits[i];
    }
    q.digits.resize(xlen - ylen + 1);
    r.digits.resize(xlen);
    divremArray (&q.digits[0], &r.digits[0], 
      (xarr == NULL ? (&x.digits[0]) : xarr), xlen, 
      (yarr == NULL ? (&y.digits[0]) : yarr), ylen);
    if (xarr != NULL) delete [] xarr;
    if (yarr != NULL) delete [] yarr;
  }
  q.normalize();
  r.normalize();
}

/******************** EXPONENTIATION ********************/

// this = this ^ x
void PosInt::pow (const PosInt& x) {
  static const PosInt one(1);

  if (this == &x) {
    PosInt xcopy(x);
    pow(xcopy);
    return;
  }

  PosInt mycopy(*this);
  set(1);
  for (PosInt i(0); i.compare(x) < 0; i.add(one)) {
    mul(mycopy);
  }
}

// result = a^b mod n
void powmod (PosInt& result, const PosInt& a, const PosInt& b, const PosInt& n) {

}

/******************** GCDs ********************/

// this = gcd(x,y)
void PosInt::gcd (const PosInt& x, const PosInt& y) {
  PosInt b(y);
  set(x);
  while (!b.isZero()) {
    PosInt r (*this);
    r.mod(b);
    set(b);
    b.set(r);
  }
}

// this = gcd(x,y) = s*x - t*y
// NOTE THE MINUS SIGN! This is required so that both s and t are
// always non-negative.
void PosInt::xgcd (PosInt& s, PosInt& t, const PosInt& x, const PosInt& y) {
  return;
}

/******************** Primality Testing ********************/

// returns true if this is PROBABLY prime
bool PosInt::MillerRabin () const {
  return false;
}
