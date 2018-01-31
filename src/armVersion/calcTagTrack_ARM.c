#include "calcTagLoc_ARM.h"
char Cal3DTrack(LPST_COL3D lpstCol3DLoc, float T, float Rx,
                LPST_TRACK3D param, char bInitLocIncluded) {
    ST_COL3D x_hat_t;
    ST_COL3D v_hat_t;
    ST_COL3D z_x_meas;
    ST_COL3D z_v_meas;
    ST_COL3D y_x_tilde;
    ST_COL3D y_v_tilde;

    float Sx;
    float Sv;
    float Kx;
    float Kv;
    float Px_pri_t;
    float Pv_pri_t;
    float Rv;
    float Q = 0.01f;

    if (TRUE == bInitLocIncluded) {
        return FALSE;
    } else {
        // 1. x = Hx + Bu
        x_hat_t.fX = param->x_t_1.fX + (param->v_t_1.fX * T);
        x_hat_t.fY = param->x_t_1.fY + (param->v_t_1.fY * T);
        v_hat_t.fX = param->v_t_1.fX;
        v_hat_t.fY = param->v_t_1.fY;
        // 2. P = FPF + Q
        Px_pri_t = param->Pxx + param->Pvv * T * T + Q;
        Pv_pri_t = param->Pvv + Q/T/T*4.f;
        // 3. y = z - Hx
        z_x_meas = *lpstCol3DLoc;
        y_x_tilde.fX = z_x_meas.fX - x_hat_t.fX;
        y_x_tilde.fY = z_x_meas.fY - x_hat_t.fY;
        z_v_meas.fX = (z_x_meas.fX - param->x_t_1.fX) / T;
        z_v_meas.fY = (z_x_meas.fY - param->x_t_1.fY) / T;
        y_v_tilde.fX = z_v_meas.fX - v_hat_t.fX;
        y_v_tilde.fY = z_v_meas.fY - v_hat_t.fY;
        // 4. S = R + HPH
        Sx = Rx + Px_pri_t;
        Rv = (Rx + param->Pxx) / (T * T);
        Sv = Rv + Pv_pri_t;
        // 5. k = PH/S
        Kx = Px_pri_t / Sx;
        Kv = Pv_pri_t / Sv;
        // 6. x = x + Ky <= x = Kx + (1-K)z
        lpstCol3DLoc->fX = x_hat_t.fX + y_x_tilde.fX * Kx;
        lpstCol3DLoc->fY = x_hat_t.fY + y_x_tilde.fY * Kx;
        param->x_t_1 = *lpstCol3DLoc;
        param->v_t_1.fX = v_hat_t.fX + y_v_tilde.fX * Kv;
        param->v_t_1.fY = v_hat_t.fY + y_v_tilde.fY * Kv;
        // 7. P = P - KHP
        param->Pxx = Px_pri_t - Px_pri_t * Kx;
        param->Pvv = Pv_pri_t - Pv_pri_t * Kv;

        return TRUE;
    }
}
