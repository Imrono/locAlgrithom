#include "uiMainWindow.h"
#include <QApplication>
#include <QSettings>
#include "calcLibMatrixOp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    dType *A_data = new dType[4];
    dType **A = new dType*[2];
    A[0] = A_data;
    A[1] = A_data + 2;
    A[0][0] = 2;
    A[0][1] = 1;
    A[1][0] = 1;
    A[1][1] = 2;
    qDebug() << A[0][0] << A[0][1];
    qDebug() << A[1][0] << A[1][1];
    dType *A_data_inverse = new dType[4];
    dType **A_inverse = new dType*[2];
    A_inverse[0] = A_data_inverse;
    A_inverse[1] = A_data_inverse + 2;
    matrix22_inverse(A, A_inverse);
    qDebug() << A_inverse[0][0] << A_inverse[0][1];
    qDebug() << A_inverse[1][0] << A_inverse[1][1];

    qDebug() << "_________________________";
    dType *A3_data = new dType[9];
    dType **A3 = new dType*[3];
    A3[0] = A3_data;
    A3[1] = A3_data + 3;
    A3[2] = A3_data + 6;
    A3[0][0] = 3;
    A3[0][1] = 7;
    A3[0][2] = -3;
    A3[1][0] = -2;
    A3[1][1] = -5;
    A3[1][2] = 2;
    A3[2][0] = -4;
    A3[2][1] = -10;
    A3[2][2] = 3;
    qDebug() << A3[0][0] << A3[0][1] << A3[0][2];
    qDebug() << A3[1][0] << A3[1][1] << A3[1][2];
    qDebug() << A3[2][0] << A3[2][1] << A3[2][2];
    dType *A3_data_inverse = new dType[9];
    dType **A3_inverse = new dType*[3];
    A3_inverse[0] = A3_data_inverse;
    A3_inverse[1] = A3_data_inverse + 3;
    A3_inverse[2] = A3_data_inverse + 6;
    matrix33_inverse(A3, A3_inverse);
    qDebug() << A3_inverse[0][0] << A3_inverse[0][1] << A3_inverse[0][2];
    qDebug() << A3_inverse[1][0] << A3_inverse[1][1] << A3_inverse[1][2];
    qDebug() << A3_inverse[2][0] << A3_inverse[2][1] << A3_inverse[2][2];
    uiMainWindow w;
    w.show();

    return a.exec();
}
