#include "calcLibMatrixOp.h"
#include <QtMath>
#include <QDebug>

void matrixSum(dType const * const * const a, dType const * const * const b,
               dType * const * const c, const long nRow, const long nCol) {
    for (long i = 0; i < nRow; i++)
        for(long j = 0; j < nCol; j++)
            c[i][j] = a[i][j] + b[i][j];
}

void matrixMuti(dType const * const * const a, dType const * const * const b,
                dType * const * const c, const long naRow, const long naCol_nbRow, const long nbCol) {
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
void matrixMuti(dType const * const * const a, const dType * const b,
                dType * const c, const long nRow, const long nCol) {
    for (long i = 0; i < nRow; i++)
        c[i] = 0.0f;
    for (long i = 0; i < nRow; i++) {
        for(long j = 0; j < nCol; j++) {
            c[i] += a[i][j] * b[j];
        }
    }
}

void matrixTrans(dType const * const * const A, dType * const * const AT, const long nRow, const long nCol) {
    for(int i{0}; i < nRow; i++)
        for(int j{0}; j < nCol; j++)
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
            //qDebug() << "###" << A[i][k] << A[k][k];
        }
        //qDebug() << "c[" << i-1 << "]" << c[i-1];
        //第K次的消元计算
        for (i = k + 1; i < n; i++) {
            for (j = 0; j < n; j++) {
                A[i][j] -= c[i] * A[k][j];
            }
            b[i] -= c[i] * b[k];
        }
    }
    //qDebug() << A[0][0] << A[0][1] << "|" << b[0];
    //qDebug() << A[1][0] << A[1][1] << "|" << b[1];

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

bool leastSquare(dType const * const * const A, dType const * const b,
		 dType * const x, long nRow, long nCol, dType lamda) {
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
    //qDebug() << "AT"
    //         << AT[0][0] << AT[0][1] << AT[0][2]
    //         << AT[1][0] << AT[1][1] << AT[1][2];
    matrixMuti(AT, A, ATA, nCol, nRow, nCol);

    // Levenberg–Marquardt method for convergence acceleration
    for (int i = 0; i < nCol; i++) {
	ATA[i][i] *= (lamda + 1.f);
	//ATA[i][i] += lamda;
    }
    //qDebug() << "ATA"
    //         << ATA[0][0] << ATA[0][1]
    //         << ATA[1][0] << ATA[1][1];
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
bool weightedLeastSquare(dType const * const * const A, dType const * const b, dType const * const w,
			 dType * const x, long nRow, long nCol, dType lamda) {
    dType **tmpA = new dType*[nRow];
    for (int i = 0; i < nRow; i++) {
	tmpA[i] = new dType[nCol];
    }
    dType *tmpB = new dType[nRow];

    // refill matrix with weight
    for (int i = 0; i < nRow; i++) {
	for (int j = 0; j < nCol; j++) {
	    tmpA[i][j] = w[i]*A[i][j];
	    //qDebug() << "[" << i << "," << j << "]" << tmpA[i][j] << A[i][j] << "|" << w[i];
	}
	tmpB[i] = w[i]*b[i];
	//qDebug() << tmpB[i] << b[i] << w[i];
    }

    bool ans = leastSquare(tmpA, tmpB, x, nRow, nCol, lamda);

    delete []tmpB;
    for (int i = 0; i < nRow; i++) {
	delete []tmpA[i];
    }
    delete []tmpA;
    return ans;
}

bool leastSquare_A(dType **ATA, dType **AT, dType *b, dType *x, long A_Row) {
    if (A_Row < 3)
        return false;

    dType *ATb = new dType[3];

    matrixMuti(AT, b, ATb, 3, A_Row);
    bool ans = gaussianElimination(ATA, ATb, x, 3);

    delete []ATb;

    return ans;
}

void coefficient_B(dType const * const * const A,
                   dType * const * const coA, long nRow, long nCol) {
    if (nRow < nCol)
        return;

    dType **AT = new dType*[nCol];
    for (int i = 0; i < nCol; i++) {
        AT[i] = new dType[nRow];
    }
    dType **ATA = new dType*[nCol];
    for (int i = 0; i < nCol; i++) {
        ATA[i] = new dType[nCol];
    }
    dType **tmp = new dType*[nCol];
    for (int i = 0; i < nCol; i++) {
        tmp[i] = new dType[nCol];
    }
    dType **tmp_1 = new dType*[nCol];
    for (int i = 0; i < nCol; i++) {
        tmp_1[i] = new dType[nCol];
    }

    matrixTrans(A, AT, nRow, nCol);
    //qDebug() << "[coefficient_B] AT => row:" << nCol <<  "col:" << nRow;
    //for (int i = 0; i < nCol; i++) {
    //    qDebug() << AT[i][0] << AT[i][1] << AT[i][2] << AT[i][3] << AT[i][4];
    //}
    //qDebug() << "AT_ls:" << nCol << nRow;
    //qDebug() << AT[0][0] << AT[0][1] << AT[0][2]
    //         << AT[1][0] << AT[1][1] << AT[1][2];
    dType *I = new dType[nCol];
    for (int i = 0; i < nCol; i++) {
        for (int j = 0; j < nCol; j++) {
            if(i == j)
                I[j] = 1.f;
            else
                I[j] = 0.f;
        }
        matrixMuti(AT, A, ATA, nCol, nRow, nCol);
        //qDebug() << "ATA_ls:" << nCol << nCol;
        //qDebug() << ATA[0][0] << ATA[0][1]
        //         << ATA[1][0] << ATA[1][1];
        //qDebug() << "I" << I[0] << I[1];
        gaussianElimination(ATA, I, tmp[i], nCol);
        //qDebug() << "tmp_ls:" << nCol;
        //qDebug() << tmp[i][0] << tmp[i][1];
    }
    delete I;

    matrixTrans(tmp, ATA, nCol, nCol);
    //qDebug() << "ATA^-1_ls:" << nCol << nCol;
    //qDebug() << ATA[0][0] << ATA[0][1]
    //         << ATA[1][0] << ATA[1][1];
    matrixMuti(ATA, AT, coA, nCol, nCol, nRow);

    matrixMuti(AT, A, tmp, nCol, nRow, nCol);
    matrixMuti(ATA, tmp, tmp_1, nCol, nCol, nCol);
    //qDebug() << "tmp_1_ls:" << nCol << nCol;
    //qDebug() << tmp_1[0][0] << tmp_1[0][1]
    //         << tmp_1[1][0] << tmp_1[1][1];

    for (int i = 0; i < nCol; i++) {
        delete []tmp_1[i];
    }
    delete []tmp_1;
    for (int i = 0; i < nCol; i++) {
        delete []tmp[i];
    }
    delete []tmp;
    for (int i = 0; i < nCol; i++) {
        delete []ATA[i];
    }
    delete []ATA;
    for (int i = 0; i < nCol; i++) {
        delete []AT[i];
    }
    delete []AT;
}

dType calcMSE(dType **A, dType *b, dType *x, long nRow, long nCol) {
    dType ans = 0.f;
    dType tmp = 0.f;
    for (long i = 0; i < nRow; i++) {
        tmp = 0.f;
        for(long j = 0; j < nCol; j++) {
            tmp += A[i][j] * x[j];
        }
        ans += qPow(tmp - b[i], 2);
    }
    return qSqrt(ans/nRow);
}
