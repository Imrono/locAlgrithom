#ifndef CALC_LIB_MATH_H
#define CALC_LIB_MATH_H
#include "_myheads.h"
#include "datatype.h"

void sortDistance(const int *distance, int /*OUT*/*idx, const int N);
void sortDistance(const int *distance, int *outDist, int *outIdx, const int N);

int combination(int up, int down);
void randomDraw(QVector<QVector<int>> &ans, int n, int N, int M);

double normalDistribution(QPointF test, QPointF anchor, double r, double sigma = 1.);
#endif // CALCLIBMATH_H

