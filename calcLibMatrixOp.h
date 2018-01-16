#ifndef MATRIXOP_H
#define MATRIXOP_H
#include "_myheads.h"

void matrixSum(dType const * const * const a, dType const * const * const b,
               dType * const * const c, const long nRow, const long nCol);
void matrixMuti(dType const * const * const a, dType const * const * const b,
                dType * const * const c, const long naRow, const long naCol_nbRow, const long nbCol);
void matrixMuti(dType const * const * const a, const dType *const b,
                dType * const c, const long nRow, const long nCol);
void matrixTrans(dType const * const * const A, dType * const * const AT, const long nRow, const long nCol);


bool gaussianElimination(dType **A, dType *b, dType *x, long n);
bool leastSquare(dType const * const * const A, dType const * const b,
                 dType * const x, long nRow, long nCol, dType lamda = 0.0f);

bool leastSquare_A(dType **ATA, dType **AT, dType *b, dType *x, long A_Row);

// (ATA)-1*AT
void coefficient_B(dType const * const * const A,
                   dType * const * const coA, long nRow, long nCol);

dType calcMSE(dType **A, dType *b, dType *x, long nRow, long nCol);
#endif // MATRIXOP_H
