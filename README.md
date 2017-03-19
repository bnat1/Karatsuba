# Karatsuba
Comparing the Divide and Conquer Karatsuba multiplication algorithm against the text book implementation

## Problem Description

For this project, you will need to implement Karatsuba's method, an efficient, divide-and-conquer algorithm for multiplication of large integers. The C++ class PosInt implements basic operations on multiprecision integers and includes a multiplication routine. However, the multiplication algorithm is just the standard “schoolbook” algorithm with running time (n2) to multiply two n-digit numbers.

There are two extra functions that have been defined in the PosInt class for doing “faster” multiplication. The first function is:

void PosInt::fastMul (const PosInt& x)

which is the top-level function to multiply this by the PosInt x. You should use it just like you would use the usual mul function. This function sets-up the call to fastMullArray, taking care of memory allocation and zero-padding.

Most of your time will be spent implementing the second function:

void PosInt::fastMulArray (int* dest, const int* x, const int* y, int len)

This is a static function in the PosInt class, meaning that it doesn't get called on any particular object: it's just working on arrays of ints instead of PosInt objects. This should make it easier to set up your recursive calls, since they will just be on sub-arrays of the original arrays. The function stub is provided, but you will have to review the Karatsuba mulitiplication algorithm that we discussed in class in order to implement the algorithm.

Once you get fastMul working, you need to determine the performance crossover point with “schoolbook” multiplication. That is, although we expect fastMul to be asymptotically faster than the basic algorithm implemented in mul, it could be that mul is faster for small numbers. You need to determine how big the arguments should be before you switch over to fastMul. To determine this, you will need to run multiple tests of mul and fastMul with different size arguments. You will need to analyze the timings from the tests to determine the crossover point and document your analysis in a short paper.

The PosInt Class

In cryptographic applications, we have to perform operations on very large integers that can't be stored within a single int. For example, in RSA we work with integers that are thousands of bits long. There are good, free packages available for doing this that have been developed by researchers around the world. The most popular is GNU's GMP package. However, we're going to use a much smaller multiprecision class developed by Dan Roche at the Naval Academy.

The C++ class PosInt implements basic multiprecision arithmetic operations, including addition, subraction, multiplication, and integer division with remainder; it also provides functions to create a PosInt object from a char array or integer as well as functions to print a multiprecision integer.

The class and a simple driver program are provided in posint.tar.gz. The driver provides an example of how to create and manipulate multiprecision integers using PosInt. In addition, you can look at the header file posint.h for the PosInt class to see what the interface is like. What you should notice is that you will use multiprecision integers differently than how you would use a regular int.

For example, the “dumb” way of doing exponentiation might be written in pseudocode as:

```1 def pow(n, k):
2    res = 1
3    for 1 in range(0, k):
4        res = res * n
5    return res```

Writing this in C++ using the PosInt class would look like:

```1 void pow (PosInt& res, const PosInt& n, const PosInt& k) {
2   PosInt i(1);
3   PosInt one(1);
4   res.set(1);
5   while (i.compare(k) <= 0) {
6     res.mul(n);
7     i.add(one);
8   }
9 }```

Here are some things to notice:

You can't use the usual C++ operators for arithmetic. So for example to add, you have to use the add method in the PosInt class, not the + operator. This seems annoying, but makes it easier to keep track of exactly what is happening.
There are no automatic conversions from regular ints. This means we can't do something like “i = i + 1” directly, but instead must first create a PosInt object (called one here) to hold the value 1, and add that to i.
Most PosInt class member functions act like the “in-place” arithmetic operators such as *= and +=. In the example above, doing res.mul(n); means to set res to the result of multiplying res times n.
Most non-member functions that deal with PosInt objects do not actually return anything. Instead, they take the return value as the first parameter to the function, which is set by the function. The reason for this is to avoid too much unnecessary copying of these (potentially large) integers. So for example, the pow function above doesn't actually return the result, but instead it takes a reference to the result as the first argument.
The method set is used to set the value based on that of another PosInt or a regular int, and the convert method is used to convert a (small) PosInt back into a regular int.
Finally, while the normal I/O operators << and >> will work just fine for reading and writing PosInts, for debugging you might find the print_array method useful. This prints the digits of the actual number in an array just like we did in class, so you can see what the actual representation looks like.

Deliverables

Modified posint.cpp implementing Karatsuba's method using the functions described above.
A driver program called driver.cpp that generates timing results for mul and fastMul for arguments of varying size.
A Makefile that builds the program driver.
A brief, formal paper describing your implementation of Kartatsuba's method and your analysis to determine the crossover point for “schoolbook” multiplication versus Karatsuba's method. The paper should also include a table of all timing data generated (may be in an Appendix) and a description of the hardware used for testing.
Submission

To be determined.

Working with Others

You may work with a partner on the project. Larger groups will not be permitted.

You may discuss freely the operation of PosInt and how, conceptually, Karatsuba's method works.
