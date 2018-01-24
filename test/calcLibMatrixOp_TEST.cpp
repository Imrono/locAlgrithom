#include "calcLibMatrixOp_TEST.h"
#include <QtMath>

calcLibMatrixOp_TEST::calcLibMatrixOp_TEST()
{

}

void calcLibMatrixOp_TEST::matrix22_inverse_TEST() {
    qDebug() << "### begin ###";
    qDebug() << "_________________________";
    dType *A_data = new dType[4];
    dType **A = new dType*[2];
    A[0] = A_data;
    A[1] = A_data + 2;
    A[0][0] = 2;
    A[0][1] = 1;
    A[1][0] = 1;
    A[1][1] = 2;
    qDebug() << "matrix A:";
    qDebug() << A[0][0] << A[0][1];
    qDebug() << A[1][0] << A[1][1];
    dType *A_data_inverse = new dType[4];
    dType **A_inverse = new dType*[2];
    A_inverse[0] = A_data_inverse;
    A_inverse[1] = A_data_inverse + 2;
    matrix22_inverse(A, A_inverse);
    qDebug() << "A inverse:";
    qDebug() << A_inverse[0][0] << A_inverse[0][1];
    qDebug() << A_inverse[1][0] << A_inverse[1][1];

    // VERIFY
    dType tmp1 = 2.f/3.f;
    dType tmp2 = -1.f/3.f;
    QVERIFY(qAbs(A_inverse[0][0]-tmp1) < MY_EPS);
    QVERIFY(qAbs(A_inverse[0][1]-tmp2) < MY_EPS);
    QVERIFY(qAbs(A_inverse[1][0]-tmp2) < MY_EPS);
    QVERIFY(qAbs(A_inverse[1][1]-tmp1) < MY_EPS);

    delete []A_inverse;
    delete []A_data_inverse;
    delete []A;
    delete []A_data;
    qDebug() << "### end ###";
}

void calcLibMatrixOp_TEST::matrix33_inverse_TEST() {
    qDebug() << "### begin ###";
    qDebug() << "_________________________";
    dType *A_data = new dType[9];
    dType **A = new dType*[3];
    A[0] = A_data;
    A[1] = A_data + 3;
    A[2] = A_data + 6;
    A[0][0] = 3;
    A[0][1] = 7;
    A[0][2] = -3;
    A[1][0] = -2;
    A[1][1] = -5;
    A[1][2] = 2;
    A[2][0] = -4;
    A[2][1] = -10;
    A[2][2] = 3;
    qDebug() << A[0][0] << A[0][1] << A[0][2];
    qDebug() << A[1][0] << A[1][1] << A[1][2];
    qDebug() << A[2][0] << A[2][1] << A[2][2];
    dType *A_data_inverse = new dType[9];
    dType **A_inverse = new dType*[3];
    A_inverse[0] = A_data_inverse;
    A_inverse[1] = A_data_inverse + 3;
    A_inverse[2] = A_data_inverse + 6;
    matrix33_inverse(A, A_inverse);
    qDebug() << A_inverse[0][0] << A_inverse[0][1] << A_inverse[0][2];
    qDebug() << A_inverse[1][0] << A_inverse[1][1] << A_inverse[1][2];
    qDebug() << A_inverse[2][0] << A_inverse[2][1] << A_inverse[2][2];

    QVERIFY(qAbs(A_inverse[0][0]-5) < MY_EPS);
    QVERIFY(qAbs(A_inverse[0][1]-9) < MY_EPS);
    QVERIFY(qAbs(A_inverse[0][2]+1) < MY_EPS);
    QVERIFY(qAbs(A_inverse[1][0]+2) < MY_EPS);
    QVERIFY(qAbs(A_inverse[1][1]+3) < MY_EPS);
    QVERIFY(qAbs(A_inverse[1][2]-0) < MY_EPS);
    QVERIFY(qAbs(A_inverse[2][0]-0) < MY_EPS);
    QVERIFY(qAbs(A_inverse[2][1]-2) < MY_EPS);
    QVERIFY(qAbs(A_inverse[2][2]+1) < MY_EPS);

    delete []A_inverse;
    delete []A_data_inverse;
    delete []A;
    delete []A_data;
    qDebug() << "### end ###";
}
