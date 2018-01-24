#include "calcLibMatrixOp.h"

bool matrix22_inverse(dType **A, dType **A_inverse) {
    if (nullptr == A || nullptr == A_inverse) {
        return false;
    }
    dType det_A = A[0][0]*A[1][1] - A[0][1]*A[1][0];
    if (det_A < MY_EPS) {
        return false;
    }

    A_inverse[0][0] =  A[1][1] / det_A;
    A_inverse[0][1] = -A[0][1] / det_A;
    A_inverse[1][0] = -A[1][0] / det_A;
    A_inverse[1][1] =  A[0][0] / det_A;
    return true;
}

bool matrix33_inverse(dType **A, dType **A_inverse) {
    if (nullptr == A || nullptr == A_inverse) {
        return false;
    }
    dType det_A =
          + A[0][0]*(A[1][1]*A[2][2]-A[1][2]*A[2][1])
          - A[1][0]*(A[0][1]*A[2][2]-A[0][2]*A[2][1])
          + A[2][0]*(A[0][1]*A[1][2]-A[0][2]*A[1][1]);
    if (det_A < MY_EPS) {
        return false;
    }

    A_inverse[0][0] =  (A[1][1]*A[2][2]-A[1][2]*A[2][1]) / det_A;
    A_inverse[0][1] =  (A[0][2]*A[2][1]-A[0][1]*A[2][2]) / det_A;
    A_inverse[0][2] =  (A[0][1]*A[1][2]-A[0][2]*A[1][1]) / det_A;
    A_inverse[1][0] =  (A[1][2]*A[2][0]-A[1][0]*A[2][2]) / det_A;
    A_inverse[1][1] =  (A[0][0]*A[2][2]-A[0][2]*A[2][0]) / det_A;
    A_inverse[1][2] =  (A[0][2]*A[1][0]-A[0][0]*A[1][2]) / det_A;
    A_inverse[2][0] =  (A[1][0]*A[2][1]-A[1][1]*A[2][0]) / det_A;
    A_inverse[2][1] =  (A[0][1]*A[2][0]-A[0][0]*A[2][1]) / det_A;
    A_inverse[2][2] =  (A[0][0]*A[1][1]-A[0][1]*A[1][0]) / det_A;

    return true;
}
