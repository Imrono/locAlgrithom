#include "calcTagPos_ARM.h"

char Cal3DLoc(LPST_COL3D lpstCol3DRef, unsigned char nRefNum, unsigned char nRealNum,
              LPST_COL3D lpstCol3DLoc, char bInitLocIncluded ) {
    float g_A_data[MAX_REF_NUM][3];
    float *g_A[MAX_REF_NUM] = {g_A_data[0], g_A_data[1], g_A_data[2], g_A_data[3], g_A_data[4],
                               g_A_data[5], g_A_data[6], g_A_data[7], g_A_data[8], g_A_data[9]};
    float g_B[MAX_REF_NUM];
    float g_X[3] = {0.f, 0.f, 0.f};
    float g_h_lm[3] = {0.f, 0.f, 0.f};
    ST_COL3D g_sortedCol3D[MAX_REF_NUM];
    float g_W[MAX_REF_NUM];

    int i = 0;
    float tmp = 0.f;
    int matrixN = nRefNum;
    int refIdx = 0;
    float refDist = 0.f;
    float currDist = 0.f;
    float diffDist = 0.f;
    float sensorDist = 0.f;
    ST_COL3D X0;    //Taylor series expansion at X0
    ST_COL3D X_new;

    // Levenberg-Marquardt damping parameter
    float nu = 2.f;
    float mu = .08f;
    float F_new = 0.f;
    float F = 0.f;
    float L_h = 0.f;
    // iteration
    int k = 0;
    int k_max = 40;
    float eps1 = 10.f;
    float eps2 = 0.02f;
    char found = FALSE;

    char rst = FALSE;

    (void) nRealNum;

    /**********************************************************************/
    // sort distance
    sortDistance(lpstCol3DRef, g_sortedCol3D, nRefNum);

    /**********************************************************************/
    // calculate weight
    refIdx = (nRefNum + 1) / 2;
    refDist = g_sortedCol3D[refIdx].fDistance;
    for (i = 0; i < nRefNum; i++) {
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

    /**********************************************************************/
    // Levenberg-Marquardt Method
    while ((FALSE == found && k++ < k_max)) {
        // Taylor series point X0
        X0 = (*lpstCol3DLoc);

        // fill the matrix
        // f(x) = f(x0) + J(x0)h + O(h^2)
        // l(h) = f(x0) + J(x0)h
        // f(x) ≈ l(h)
        for (i = 0; i < nRefNum; i++) {
            tmp =  sqrtf((X0.fX - g_sortedCol3D[i].fX) * (X0.fX - g_sortedCol3D[i].fX)
                       + (X0.fY - g_sortedCol3D[i].fY) * (X0.fY - g_sortedCol3D[i].fY));
            g_A[i][0] = ((X0.fX - g_sortedCol3D[i].fX) / tmp) * g_W[i];
            g_A[i][1] = ((X0.fY - g_sortedCol3D[i].fY) / tmp) * g_W[i];
            g_A[i][2] = ((X0.fZ - g_sortedCol3D[i].fZ) / tmp) * g_W[i];
            g_B[i]    = (g_sortedCol3D[i].fDistance - tmp)    * g_W[i];
        }

        leastSquare_ARM(g_A, g_B, g_h_lm, matrixN, 2, mu);

        X_new.fX = X0.fX + g_h_lm[0];
        X_new.fY = X0.fY + g_h_lm[1];
        X_new.fZ = X0.fZ + 0.f;

        // ##case 1## found := ||h_lm|| < ε2 * (||x|| + ε2)
        // accepted + found
        if (sqrtf(g_h_lm[0] * g_h_lm[0] + g_h_lm[1] * g_h_lm[1])
                < eps2 * (sqrtf(X0.fX * X0.fX + X0.fY * X0.fY) + eps2)) {
            found = TRUE;
            (*lpstCol3DLoc) = X_new;
            rst = 1;
            continue;
        } else {
            F_new = 0.f;
            F = 0.f;
            L_h = 0.f;
            // F(x0) = 1/2 * ∑f_i(x0)^2
            // L(h)  = 1/2 * ∑l(h)^2
            for (i = 0; i < nRefNum; i++) {
                tmp = calcDistance_ARM(&X_new, &(lpstCol3DRef[i]));
                F_new += tmp * tmp;
                tmp = calcDistance_ARM(&X0,    &(lpstCol3DRef[i]));
                F += tmp * tmp;

                tmp = tmp + (g_A[i][0] * g_h_lm[0] + g_A[i][1] * g_h_lm[1]);
                L_h += tmp * tmp;
            }
            F_new *= .5f;
            F *= .5f;
            L_h *= .5f;

            // ρ = (F(x0) - F(x_new)) / (L(0) - L(h_lm))
            float rho = (F - F_new) / (F - L_h);
            if (rho > 0.f) {  // accepted
                (*lpstCol3DLoc) = X_new;

                // g = J^T f(x0)
                // ##case 2## found := ||g||_∞ < ε1
                // found
                found = TRUE;
                for (i = 0; i < nRefNum; i++) {
                    if (g_A[i][0] * g_h_lm[0] + g_A[i][1] * g_h_lm[1] > eps1) {
                        found = FALSE;
                        break;
                    }
                }
                if (TRUE == found) {
                    rst = 2;
                    continue;
                }

                // μ := μ * max(1/3, 1-(2*ρ-1)^3)
                tmp = 2.f * rho - 1.f;
                tmp = 1 - tmp * tmp * tmp;
                mu *= 0.333333333f > tmp ? 0.333333333f : tmp;
                // ν := 2
                nu = 2.f;
            } else {        // rejected
                // μ := μ * ν
                mu *= nu;
                // ν := 2 * ν
                nu *= 2.f;
            }
        }
    };

    return rst;
}
