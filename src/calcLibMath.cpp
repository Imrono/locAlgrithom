#include "calcLibMath.h"

void sortDistance(const int *distance, int /*OUT*/*idx, const int N) {
    int ii;
    for (int i = 0; i < N; i++) {
        int tmp = 0;
        for (int j = 0; j < N; j++) {
            if (tmp < distance[j]) {
                bool found = false;
                for (int k = 0; k < i; k++) {
                    if (idx[k] == j) found = true;
                }
                if (!found) {
                    tmp = distance[j];
                    ii = j;
                } else {}
            }
        }
        idx[i] = ii;
    }
    for (int j = 0; j < N/2; j++) {
        int tmp = idx[j];
        idx[j] = idx[N-1-j];
        idx[N-1-j] = tmp;
    }
}

void sortDistance(const int *distance, int *outDist, int *outIdx, const int N) {
    int ii;
    for (int i = 0; i < N; i++) {
        int tmp = 0;
        for (int j = 0; j < N; j++) {
            if (tmp < distance[j]) {
                bool found = false;
                for (int k = 0; k < i; k++) {
                    if (outIdx[k] == j) found = true;
                }
                if (!found) {
                    tmp = distance[j];
                    ii = j;
                } else {}
            }
        }
        outIdx[i] = ii;
        outDist[i] = tmp;
    }
}

double SQR(double a){
    double x=a,y=0.0;
    while(fabs(x-y)>0.00001){
        y=x;
        x=0.5*(x+a/x);
    }
    return x;
}
