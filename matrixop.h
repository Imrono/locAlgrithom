#ifndef MATRIXOP_H
#define MATRIXOP_H

#ifndef dType
#define dType double
#endif

void matrixSum(const dType **a, const dType **b, dType **c, const long nRow, const long nCol);
void matrixMuti(dType **a, dType **b, dType **c, const long naRow, const long naCol_nbRow, const long nbCol);
void matrixMuti(dType **a, dType *b, dType *c, const long nRow, const long nCol);
void matrixTrans(dType **A, dType **AT, const long nRow, const long nCol);


bool gaussianElimination(dType **A, dType *b, dType *x, long n);
bool leastSquare(dType **A, dType *b, dType *x, long nRow, long nCol);
#endif // MATRIXOP_H
