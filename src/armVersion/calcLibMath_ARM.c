#include "calcTagPosWeightTaylor_ARM.h"

float calcDistanceMSE(const LPST_COL3D lpstCol3DRef, const float *X, const int N) {
    float currD = 0.f;
    float ans = 0.f;
    int i = 0;

    for (int i = 0; i < N; i++) {
        currD = sqrtf((X[0]-lpstCol3DRef[i].fX)*(X[0]-lpstCol3DRef[i].fX)
                    + (X[1]-lpstCol3DRef[i].fY)*(X[1]-lpstCol3DRef[i].fY));
        ans += (lpstCol3DRef[i].fDistance - currD) * (lpstCol3DRef[i].fDistance - currD);
    }
    ans /= N;

    return ans;
}

void sortDistance(const LPST_COL3D lpstCol3DRef, LPST_COL3D lpstCol3DSorted, const int N) {
    int i = 0;
    int j = 0;
    ST_COL3D tmp;

    // copy
    for (i = 0; i < N; i++) {
        lpstCol3DSorted[i] = lpstCol3DRef[i];
    }

    // Bubble Sort
    for (i = 0; i < N; i++) {
        for (j = 0; j < N-i-1; j++) {
            if (lpstCol3DSorted[j].fDistance > lpstCol3DSorted[j+1].fDistance) {
                tmp = lpstCol3DSorted[j+1];
                lpstCol3DSorted[j+1] = lpstCol3DSorted[j];
                lpstCol3DSorted[j] = tmp;
            }
        }
    }
}

void calcATA_ARM(float const * const * const A, const int row, const int col, float **ATA) {
    int i = 0;
    int j = 0;
    int k = 0;

    for (i = 0; i < col; i++) {
        for (j = 0; j < col; j++) {
            ATA[i][j] = 0.f;
            for (k = 0; k < row; k++) {
                ATA[i][j] += A[k][i] * A[k][j];
            }
        }
    }
}
void matrixMuti_ATb_ARM(float const * const * const A, float const * const b,
                        const int row, const int col, float *ATb) {
    int i = 0;
    int j = 0;

    for (i = 0; i < col; i++) {
        ATb[i] = 0;
        for (j = 0; j < row; j++) {
            ATb[i] += A[j][i] * b[j];
        }
    }
}
void matrixMuti_Ab_ARM(float const * const * const A, float const * const b,
                       const int row, const int col, float *Ab) {
    int i = 0;
    int j = 0;

    for (i = 0; i < row; i++) {
        Ab[i] = 0;
        for (j = 0; j < col; j++) {
            Ab[i] += A[j][i] * b[j];
        }
    }
}
char matrix22_inverse_ARM(float **A, float **A_inverse) {
    if (0 == A || 0 == A_inverse) {
        return FALSE;
    }
    float det_A = A[0][0]*A[1][1] - A[0][1]*A[1][0];
    if (det_A < MY_EPS) {
        return FALSE;
    }

    A_inverse[0][0] =  A[1][1] / det_A;
    A_inverse[0][1] = -A[0][1] / det_A;
    A_inverse[1][0] = -A[1][0] / det_A;
    A_inverse[1][1] =  A[0][0] / det_A;
    return TRUE;
}

char matrix33_inverse_ARM(float **A, float **A_inverse) {
    if (0 == A || 0 == A_inverse) {
        return FALSE;
    }
    float det_A =
          + A[0][0]*(A[1][1]*A[2][2]-A[1][2]*A[2][1])
          - A[1][0]*(A[0][1]*A[2][2]-A[0][2]*A[2][1])
          + A[2][0]*(A[0][1]*A[1][2]-A[0][2]*A[1][1]);
    if (det_A < MY_EPS) {
        return FALSE;
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

    return TRUE;
}
char leastSquare_ARM(float const * const * const A, float const * const b,
                     float * const x, long nRow, long nCol, float lamda) {
    if (nRow < nCol)
        return FALSE;

    char ans = TRUE;
    float ATA_data[4][4];
    float *ATA[4];
    ATA[0] = ATA_data[0];
    ATA[1] = ATA_data[1];
    ATA[2] = ATA_data[2];
    ATA[3] = ATA_data[3];

    float ATb[4];
    float ATA_inverse_data[4][4];
    float *ATA_inverse[4];
    ATA_inverse[0] = ATA_inverse_data[0];
    ATA_inverse[1] = ATA_inverse_data[1];
    ATA_inverse[2] = ATA_inverse_data[2];
    ATA_inverse[3] = ATA_inverse_data[3];

    calcATA_ARM(A, nRow, nCol, ATA);

    // Levenberg–Marquardt method for convergence acceleration
    for (int i = 0; i < nCol; i++) {
        ATA[i][i] *= (lamda + 1.f);
        //ATA[i][i] += lamda;
    }

    matrixMuti_ATb_ARM(A, b, nRow, nCol, ATb);

    if (2 == nCol) {
        ans = matrix22_inverse_ARM(ATA, ATA_inverse);
    } else if (3 == nCol) {
        ans = matrix33_inverse_ARM(ATA, ATA_inverse);
    } else {
        ans = FALSE;
    }

    if (TRUE == ans) {
        matrixMuti_Ab_ARM(ATA_inverse, ATb, nCol, nCol, x);
    }
    return ans;
}
