#include "calcTagPos_ARM.h"

char Cal3DLoc(LPST_COL3D lpstCol3DRef, unsigned char nRefNum, unsigned char nRealNum,
              LPST_COL3D lpstCol3DLoc, char bInitLocIncluded ) {
    float g_A_data[MAX_REF_NUM][3];
    float *g_A[MAX_REF_NUM] = {g_A_data[0], g_A_data[1], g_A_data[2], g_A_data[3], g_A_data[4],
                               g_A_data[5], g_A_data[6], g_A_data[7], g_A_data[8], g_A_data[9]};
    float g_B[MAX_REF_NUM];
    float g_X[3] = {0.f, 0.f, 0.f};
    float dX[3] = {0.f, 0.f, 0.f};
    ST_COL3D g_sortedCol3D[MAX_REF_NUM];
    float g_W[MAX_REF_NUM];

    float mse = 0.f;        // mse = Δd^T Δd
    float mseKeep = 0.f;    // last accepted x's mse
    int i = 0;
    float tmp = 0.f;
    int matrixN = nRefNum;
    int refIdx = 0;
    float refDist = 0.f;
    float currDist = 0.f;
    float diffDist = 0.f;
    float sensorDist = 0.f;
    ST_COL3D X_new;

    // Levenberg-Marquardt damping parameter
    float lamda = .1f;
    // iteration
    int k = 0;
    int k_max = 15;
    float eps1 = 0.002f;
    float eps2 = 4.f;
    float eps3 = 10000.f;

    char rst = FALSE;

    (void) nRealNum;
    lpstCol3DLoc->fZ = 0.f; // 2 dimantion
    /**********************************************************************/
    // sort distance
    sortDistance(lpstCol3DRef, g_sortedCol3D, nRefNum);

    /**********************************************************************/
    // calculate weight
    refIdx = (nRefNum + 1) / 2;
    refDist = g_sortedCol3D[refIdx].fDistance;
    for (i = 0; i < nRefNum; i++) {
         g_sortedCol3D[i].fZ = 0.f; // 2 dimantion
        currDist = g_sortedCol3D[i].fDistance;
        diffDist = fabsf(currDist-refDist) + 1.f;
        if (i < refIdx) {
            sensorDist = calcDistance_ARM(&lpstCol3DRef[refIdx-1], &lpstCol3DRef[i]);
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

    /**********************************************************************/
    // initial point
    if (TRUE == bInitLocIncluded) {
        for (int i = 0; i < nRefNum; i++) {
            g_A[i][0] = -2.f*g_sortedCol3D[i].fX * g_W[i];
            g_A[i][1] = -2.f*g_sortedCol3D[i].fY * g_W[i];
            g_A[i][2] =  1.f                     * g_W[i];
        }
        for (int i = 0; i < nRefNum; i++) {
            g_B[i] = (g_sortedCol3D[i].fDistance * g_sortedCol3D[i].fDistance
                    - g_sortedCol3D[i].fX * g_sortedCol3D[i].fX
                    - g_sortedCol3D[i].fY * g_sortedCol3D[i].fY) * g_W[i];
        }
        leastSquare_ARM(g_A, g_B, g_X, matrixN, 3, 0.f);
        lpstCol3DLoc->fX = g_X[0];
        lpstCol3DLoc->fY = g_X[1];
        lpstCol3DLoc->fZ = 0.f;
    }
    mseKeep = calcDistanceMSE(g_sortedCol3D, lpstCol3DLoc, matrixN);

    /**********************************************************************/
    // Levenberg-Marquardt Method
    while (k++ < k_max) {
        // fill the matrix
        // f(x) = f(x0) + J(x0)Δx + O(h^2)
        // l(h) = f(x0) + J(x0)Δx
        // f(x) ≈ l(h) => J(x0)Δx ≈ f(x) - f(x0) = Δd
        for (i = 0; i < nRefNum; i++) {
            tmp =  sqrtf((lpstCol3DLoc->fX - g_sortedCol3D[i].fX) * (lpstCol3DLoc->fX - g_sortedCol3D[i].fX)
                       + (lpstCol3DLoc->fY - g_sortedCol3D[i].fY) * (lpstCol3DLoc->fY - g_sortedCol3D[i].fY));
            g_A[i][0] = ((lpstCol3DLoc->fX - g_sortedCol3D[i].fX) / tmp) * g_W[i];
            g_A[i][1] = ((lpstCol3DLoc->fY - g_sortedCol3D[i].fY) / tmp) * g_W[i];
            g_A[i][2] = ((lpstCol3DLoc->fZ - g_sortedCol3D[i].fZ) / tmp) * g_W[i];
            g_B[i]    = (g_sortedCol3D[i].fDistance - tmp)               * g_W[i];
        }

        leastSquare_ARM(g_A, g_B, dX, matrixN, 2, lamda);

        X_new.fX = lpstCol3DLoc->fX + dX[0];
        X_new.fY = lpstCol3DLoc->fY + dX[1];
        X_new.fZ = lpstCol3DLoc->fZ;

        // mse = Δd^T Δd
        mse = calcDistanceMSE(g_sortedCol3D, &X_new, matrixN);

        // if mse decreased, accept
        if (mse < mseKeep) {    // accept condition
            // update x0 for next iteration
            (*lpstCol3DLoc) = X_new;

            // iteration conditions
            if (mse < eps1
             || sqrtf(dX[0] * dX[0] + dX[1] * dX[1]) < eps2
             || fabsf(mse - mseKeep) < eps1 * eps3) {
                break;
            }

            mseKeep = mse;
            lamda *= 0.9f;
        } else {                // else reject
            lamda *= 1.1f;
        }
    };

    return rst;
}
