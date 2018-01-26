#include "calcTagPos_ARM.h"

char Cal3DLoc(LPST_COL3D lpstCol3DRef, unsigned char nRefNum, unsigned char nRealNum,
              LPST_COL3D lpstCol3DLoc, char bInitLocIncluded ) {
    float g_A_data[MAX_REF_NUM][3];
    float *g_A[MAX_REF_NUM] = {g_A_data[0], g_A_data[1], g_A_data[2], g_A_data[3], g_A_data[4],
                               g_A_data[5], g_A_data[6], g_A_data[7], g_A_data[8], g_A_data[9]};
    float g_B[MAX_REF_NUM];
    float g_X[3] = {0.f, 0.f, 0.f};
    float g_dX[3] = {0.f, 0.f, 0.f};
    ST_COL3D g_sortedCol3D[MAX_REF_NUM];
    float g_W[MAX_REF_NUM];

    int i = 0;
    float tmpD = 0.f;
    float mse = 0.f;
    float mseLast = 0.f;
    int matrixN = nRefNum;
    int refIdx = 0;
    float refDist = 0.f;
    float currDist = 0.f;
    float diffDist = 0.f;
    float sensorDist = 0.f;
    float X0[2];    //Taylor series expansion at X0

    // Marquardt damping parameter using trust region
    float minLamda = .5f;
    float maxLamda = 1.f;
    float nu = 1.1f;
    float mse_pref = 10000.f;
    float alpha = 1.2f;
    float beta = 50.f;
    int maxCount = 40;
    // iteration
    int count = 0;
    float lamda = minLamda;

    (void) nRealNum;

    // sort distance
    sortDistance(lpstCol3DRef, g_sortedCol3D, nRefNum);

    // calculate weight
    refIdx = (nRefNum + 1) / 2;
    refDist = g_sortedCol3D[refIdx].fDistance;
    for (i = 0; i < nRefNum; i++) {
        currDist = g_sortedCol3D[i].fDistance;
        diffDist = fabsf(currDist-refDist) + 1.f;
        if (i < refIdx) {
            sensorDist = calcDistance(&lpstCol3DRef[refIdx-1], &lpstCol3DRef[i]);
            if(fabsf(g_sortedCol3D[refIdx-1].fDistance - sensorDist) * 0.1f < currDist) {
                g_W[i] = 1.f + 0.01f * diffDist;
            } else {
                g_W[i] = InvSqrt(diffDist);
            }
        } else {
            if (g_sortedCol3D[i].fDistance > refDist * 1.4f) {
                matrixN --;
                g_W[i] = 0.f;
            } else {
                g_W[i] = InvSqrt(diffDist);
            }
        }
    }

    // initial point
    if (FALSE == bInitLocIncluded) {
        g_X[0] = lpstCol3DLoc->fX;
        g_X[1] = lpstCol3DLoc->fY;
        g_X[2] = lpstCol3DLoc->fZ;
    } else {
        for (int i = 0; i < nRefNum; i++) {
            g_A[i][0] = -2.f*g_sortedCol3D[i].fX * g_W[i];
            g_A[i][1] = -2.f*g_sortedCol3D[i].fY * g_W[i];
            g_A[i][2] =  1.f                     * g_W[i];
            //printf("%f %f %f\n", g_A[i][0], g_A[i][1], g_A[i][2]);
            //fflush(stdout);
        }
        for (int i = 0; i < nRefNum; i++) {
            g_B[i] = (g_sortedCol3D[i].fDistance * g_sortedCol3D[i].fDistance
                    - g_sortedCol3D[i].fX * g_sortedCol3D[i].fX
                    - g_sortedCol3D[i].fY * g_sortedCol3D[i].fY) * g_W[i];
            //printf("%f\n", g_B[i]);
            //fflush(stdout);
        }
        leastSquare_ARM(g_A, g_B, g_X, matrixN, 3, 0.f);
        //printf("%f %f %d\n", g_X[0], g_X[1], matrixN);
        //fflush(stdout);
    }
    g_X[2] = 0.f;
    mse = calcDistanceMSE(g_sortedCol3D, g_X, matrixN);

    do {
        X0[0] = g_X[0]; X0[1] = g_X[1];
        mseLast = mse;
        // fill the matrix
        for (i = 0; i < nRefNum; i++) {
            tmpD = sqrtf((g_X[0] - g_sortedCol3D[i].fX) * (g_X[0] - g_sortedCol3D[i].fX)
                       + (g_X[1] - g_sortedCol3D[i].fY) * (g_X[1] - g_sortedCol3D[i].fY));
            g_A[i][0] = ((g_X[0] - g_sortedCol3D[i].fX) / tmpD) * g_W[i];
            g_A[i][1] = ((g_X[1] - g_sortedCol3D[i].fY) / tmpD) * g_W[i];
            g_B[i]    = (g_sortedCol3D[i].fDistance - tmpD)     * g_W[i];
        }

        leastSquare_ARM(g_A, g_B, g_dX, matrixN, 2, lamda);

        g_X[0] = X0[0] + g_dX[0];
        g_X[1] = X0[1] + g_dX[1];
        mse = calcDistanceMSE(g_sortedCol3D, g_X, matrixN);

        if (1 == count % 5) {
            if (mse < mse_pref || mseLast > mse * alpha) {
                if (fabsf(lamda - minLamda) < MY_EPS) {
                    // do nothing
                } else {
                    lamda /= nu;
                }
            } else if (mse < mse_pref * beta) {
                // do nothing
            } else {
                if (lamda > maxLamda) {
                    // do nothing
                } else {
                    lamda *= nu;
                }
            }
        }

    } while (mse > mse_pref && count++ < maxCount && fabsf(mseLast - mse) > mse_pref/100.f);

    // output
    lpstCol3DLoc->fX = g_X[0];
    lpstCol3DLoc->fY = g_X[1];
    lpstCol3DLoc->fZ = g_X[2];

    return TRUE;
}
