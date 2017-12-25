#include "matrixop.h"
#include <QDebug>

void matrixSum(const dType **a, const dType **b, dType **c, const long nRow, const long nCol) {
    for (long i = 0; i < nRow; i++)
        for(long j = 0; j < nCol; j++)
            c[i][j] = a[i][j] + b[i][j];
}

void matrixMuti(dType **a, dType **b, dType **c, const long naRow, const long naCol_nbRow, const long nbCol) {
    for (long i = 0; i < naRow; i++) {
        for(long j = 0; j < nbCol; j++) {
            c[i][j] = 0.0f;
        }
    }
    for (long i = 0; i < naRow; i++) {
        for (long j = 0; j < naCol_nbRow; j++) {
            for(long k = 0; k < nbCol; k++) {
                c[i][k] += a[i][j] * b[j][k];
            }
        }
    }
}
void matrixMuti(dType **a, dType *b, dType *c, const long nRow, const long nCol) {
    for (long i = 0; i < nRow; i++)
        c[i] = 0.0f;
    for (long i = 0; i < nRow; i++) {
        for(long j = 0; j < nCol; j++) {
            c[i] += a[i][j] * b[j];
        }
    }
}
void matrixTrans(dType **A, dType **AT, const long nRow, const long nCol) {
    for(int i = 0; i < nRow; i++)
        for(int j = 0; j < nCol; j++)
            AT[j][i] = A[i][j];
}

bool gaussianElimination(dType **A, dType *b, dType *x, long n) {
    //判断能否用高斯消元法，如果矩阵主对角线上有0元素存在是不能用的
    for (int i = 0; i < n; i++)
        if (A[i][i] == 0)
            return false;

    int i, j, k;
    dType *c = new dType[n]{0};    //存储初等行变换的系数，用于行的相减
    //消元的整个过程如下，总共n-1次消元过程。
    for (k = 0; k < n - 1; k++) {
        //求出第K次初等行变换的系数
        for (i = k + 1; i < n; i++) {
            c[i] = A[i][k] / A[k][k];
        }
        //第K次的消元计算
        for (i = k + 1; i < n; i++) {
            for (j = 0; j < n; j++) {
                A[i][j] -= c[i] * A[k][j];
            }
            b[i] -= c[i] * b[k];
        }
    }
    //qDebug() << A[0][0] << A[0][1] << A[0][2] << "|" << b[0];
    //qDebug() << A[1][0] << A[1][1] << A[1][2] << "|" << b[1];
    //qDebug() << A[2][0] << A[2][1] << A[2][2] << "|" << b[2];

    //先计算出最后一个未知数；
    x[n - 1] = b[n - 1] / A[n - 1][n - 1];
    //求出每个未知数的值
    for (i = n - 2; i >= 0; i--) {
        dType sum = 0;
        for (j = i + 1; j < n; j++) {
            sum += A[i][j] * x[j];
        }
        x[i] = (b[i] - sum) / A[i][i];
    }
    delete c;
    return true;
}

bool leastSquare(dType **A, dType *b, dType *x, long nRow, long nCol) {
    if (nRow < nCol)
        return false;

    dType **AT = new dType*[nCol];
    for (int i = 0; i < nCol; i++) {
        AT[i] = new dType[nRow];
    }
    dType **ATA = new dType*[nCol];
    for (int i = 0; i < nCol; i++) {
        ATA[i] = new dType[nCol];
    }
    dType *ATb = new dType[nCol];

    //qDebug() << "A";
    //qDebug() << A[0][0] << A[0][1] << A[0][2] << "|" << b[0];
    //qDebug() << A[1][0] << A[1][1] << A[1][2] << "|" << b[1];
    //qDebug() << A[2][0] << A[2][1] << A[2][2] << "|" << b[2];
    matrixTrans(A, AT, nRow, nCol);
    //qDebug() << "AT";
    //qDebug() << AT[0][0] << AT[0][1] << AT[0][2];
    //qDebug() << AT[1][0] << AT[1][1] << AT[1][2];
    //qDebug() << AT[2][0] << AT[2][1] << AT[2][2];
    matrixMuti(AT, A, ATA, nCol, nRow, nCol);
    //qDebug() << "ATA";
    //qDebug() << ATA[0][0] << ATA[0][1] << ATA[0][2];
    //qDebug() << ATA[1][0] << ATA[1][1] << ATA[1][2];
    //qDebug() << ATA[2][0] << ATA[2][1] << ATA[2][2];
    matrixMuti(AT, b, ATb, nCol, nRow);
    //qDebug() << "ATb";
    //qDebug() << ATb[0];
    //qDebug() << ATb[1];
    //qDebug() << ATb[2];
    bool ans = gaussianElimination(ATA, ATb, x, nCol);

    delete []ATb;
    for (int i = 0; i < nCol; i++) {
        delete []ATA[i];
    }
    delete []ATA;
    for (int i = 0; i < nCol; i++) {
        delete []AT[i];
    }
    delete []AT;
    return ans;
}
