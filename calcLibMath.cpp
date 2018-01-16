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

