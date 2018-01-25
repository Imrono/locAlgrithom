#include "calcTagPosWeightTaylor_ARM.h"
#include <stdio.h>
char Cal3DLoc(LPST_COL3D lpstCol3DRef, unsigned char nRefNum, unsigned char nRealNum,
              LPST_COL3D lpstCol3DLoc, char bInitLocIncluded ) {
    (void) nRealNum;
    unsigned int i = 0;
    float g_A_data[MAX_REF_NUM][3];
    float *g_A[MAX_REF_NUM];
    float g_B[MAX_REF_NUM];
    float g_X[3] = {0.f, 0.f, 0.f};
    float g_dX[3] = {0.f, 0.f, 0.f};
    ST_COL3D g_sortedCol3D[MAX_REF_NUM];
    float g_W[MAX_REF_NUM];
    // INITIAL
    for (i = 0; i < MAX_REF_NUM; i++) {
        g_A[i] = &(g_A_data[i][0]);
    }

    float tmpD = 0.f;
    float mse = 0.f;
    float mseLast = 0.f;

    // sort distance
    sortDistance(lpstCol3DRef, g_sortedCol3D, nRefNum);

    // calculate weight
    int refIdx = (nRefNum + 1) / 2;
    float midDist = g_sortedCol3D[refIdx].fDistance;
    float littleDist = g_sortedCol3D[1].fDistance;
    for (int i = 0; i < nRefNum; i++) {
        float currDist = g_sortedCol3D[i].fDistance;
        if (currDist < midDist
         && currDist > 0.3f * littleDist) {
            g_W[i] = powf(fabsf(currDist-midDist)+1.f, 0.1f);
        } else {
            g_W[i] = 1.f / sqrtf(fabsf(currDist-midDist)+1.f);
        }
    }
    int nUnuseableNlos = 0;
    for (int i = 0; i < nRefNum; i++) {
        if (i < refIdx+1) {
            continue;
        } else {
            if (g_sortedCol3D[i].fDistance > g_sortedCol3D[refIdx].fDistance * 1.4f) {
                nUnuseableNlos++;
                g_W[i] = 0.f;
            } else {}
        }
    }

    int matrixN = nRefNum - nUnuseableNlos;

    // initial point
    if (FALSE == bInitLocIncluded) {
        g_X[0] = lpstCol3DLoc->fX;
        g_X[1] = lpstCol3DLoc->fY;
        g_X[2] = lpstCol3DLoc->fZ;
    } else {
        for (int i = 0; i < nRefNum; i++) {
            g_A[i][0] = -2.f*g_sortedCol3D[i].fX;
            g_A[i][1] = -2.f*g_sortedCol3D[i].fY;
            g_A[i][2] = 1.f;
        }
        for (int i = 0; i < nRefNum; i++) {
            g_B[i] = g_sortedCol3D[i].fDistance * g_sortedCol3D[i].fDistance
                   - g_sortedCol3D[i].fX * g_sortedCol3D[i].fX
                   - g_sortedCol3D[i].fY * g_sortedCol3D[i].fY;
        }
        leastSquare_ARM(g_A, g_B, g_X, matrixN, 3, 0.f);
    }
    g_X[2] = 0.f;
    mse = calcDistanceMSE(g_sortedCol3D, g_X, matrixN);

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
    do {
        float X0[2];    //迭代初值，由于有dX，所以要用X0暂存。
        X0[0] = g_X[0]; X0[1] = g_X[1];
        mseLast = mse;
        // fill the matrix
        for (int i = 0; i < nRefNum; i++) {
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
