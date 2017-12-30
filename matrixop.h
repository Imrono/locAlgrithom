#ifndef MATRIXOP_H
#define MATRIXOP_H
#include "_myheads.h"

void matrixSum(dType **a, dType **b, dType **c, const long nRow, const long nCol);
void matrixMuti(dType **a, dType **b, dType **c, const long naRow, const long naCol_nbRow, const long nbCol);
void matrixMuti(dType **a, dType *b, dType *c, const long nRow, const long nCol);
void matrixTrans(dType **A, dType **AT, const long nRow, const long nCol);


bool gaussianElimination(dType **A, dType *b, dType *x, long n);
bool leastSquare(dType **A, dType *b, dType *x, long nRow, long nCol);

bool leastSquare_A(dType **ATA, dType **AT, dType *b, dType *x, long A_Row);

// (ATA)-1*AT
void coefficient_B(dType **A, dType **coA, long nRow, long nCol);

dType calcMSE(dType **A, dType *b, dType *x, long nRow, long nCol);
#endif // MATRIXOP_H
