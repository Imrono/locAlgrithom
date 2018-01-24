#ifndef MATRIXOP_H
#define MATRIXOP_H
#define dType double
#define MY_EPS 0.000001
/******************************************************/
bool matrix22_inverse(dType **A, dType **A_inverse);
bool matrix33_inverse(dType **A, dType **A_inverse);

#endif // MATRIXOP_H
