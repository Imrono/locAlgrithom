#ifndef CALCTAGPOS_WEIGHTTAYLOR_ARMVERSION_H
#define CALCTAGPOS_WEIGHTTAYLOR_ARMVERSION_H
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define MAX_REF_NUM 10
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef MY_EPS
#define MY_EPS 0.000001f
#endif

typedef struct tagCol3D
{
    float fX;				// x coordination of reference node
    float fY;				// y coordination of reference node
    float fZ;				// z coordination of reference node
    float fDistance;		// distance to reference node
} ST_COL3D, *LPST_COL3D;

typedef struct tagTrackCol3D {  // used for next point calulate
    float Pxx;
    float Pvv;
    ST_COL3D v_t_1;
    ST_COL3D x_t_1;
} ST_TRACK3D, *LPST_TRACK3D;

float InvSqrt(float x);
float calcDistance_ARM(const LPST_COL3D p1, const LPST_COL3D p2);
float calcDistance2_ARM(float *a, float *b, int N);
float calcDistanceMSE(const LPST_COL3D lpstCol3DRef, const LPST_COL3D X, const int N);
void sortDistance(const LPST_COL3D lpstCol3DRef, LPST_COL3D lpstCol3DSorted, const int N);

void calcATA_ARM(float const * const * const A, const int row, const int col, float **ATA);
void matrixMuti_ATb_ARM(float const * const * const A, float const * const b,
                        const int row, const int col, float *ATb);
void matrixMuti_Ab_ARM(float const * const * const A, float const * const b,
                       const int row, const int col, float *Ab);
char matrix22_inverse_ARM(float **A, float **A_inverse);
char matrix33_inverse_ARM(float **A, float **A_inverse);
char leastSquare_ARM(float const * const * const A, float const * const b,
                     float * const x, long nRow, long nCol, float lamda);

/******************************************************************************/
// LPST_COL3D lpstCol3DRef : 输入结构数组，包含探测器坐标和标签到探测器的距离
// unsigned char nRefNum   : 输入结构数量，及参考节点数量
// unsigned char nRealNum  : 输入结构实际有效数量 (可以先不管这个参数，自己在程序里处理，后面在优化）
// LPST_COL3D lpstCol3DLoc : 输入上一次位置，输出此次计算新位置
// char bInitLocIncluded   : TRUE表示首次计算，需要初始化
extern char Cal3DLoc(LPST_COL3D lpstCol3DRef, unsigned char nRefNum, unsigned char nRealNum,
                     LPST_COL3D lpstCol3DLoc, char bInitLocIncluded);
/******************************************************************************/
extern char Cal3DTrack(LPST_COL3D lpstCol3DLoc, float T, float Rx, LPST_TRACK3D param, char bInitLocIncluded);
/******************************************************************************/
#endif // CALCTAGPOS_WEIGHTTAYLOR_ARMVERSION_H
