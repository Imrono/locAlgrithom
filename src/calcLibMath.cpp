#include "calcLibMath.h"
#include <QTime>
#include <QtMath>

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

int combination(int up, int down) {
    int ans = 1;
    for (int i = 0; i < up; i++) {
        ans *= (down - i) ;
    }
	for (int i = 0; i < up; i++) {
		ans /= (up - i);
	}
    return ans;
}

void randomDraw(QVector<QVector<int>> &ans, int n, int N, int M) {
    QVector<QVector<int>> idx_tot;
    QVector<int> tmpIdx_tot;

    int M_tot = combination(n, N);
    for (int i = 0; i < M_tot; i++) {
        tmpIdx_tot.append(i);
    }
    srand(static_cast<unsigned>(QTime::currentTime().msec()));
    for (int i = M_tot-1; i > 0; i--) {
        int randIdx = qrand()%i;
        int tmp = tmpIdx_tot[i];
        tmpIdx_tot[i] = tmpIdx_tot[randIdx];
        tmpIdx_tot[randIdx] = tmp;
    }

    for (int i = 0; i < M; i++) {
        // get random index
        int randIdx = qrand()%(M_tot-i);
        int tmpIdx = tmpIdx_tot[randIdx];
        tmpIdx_tot.remove(randIdx);

        // translate
        QVector<int> oneAns;
        int up = n-1;
        int down = N-1;
		int beg = 0;
        for (int k = 0; k < n - 1; k++) {
			int tmpAll = 0;
            for (int j = 0; j < (down)-(up)+1; j++) {
                int tmp = combination(up, down-j);
                if (tmpAll <= tmpIdx && tmpIdx < tmpAll+tmp) {
                    oneAns.append(j + beg);
                    up -= 1;
					down -= (j + 1);
					beg += (j + 1);
					break;
                } else {
					tmpAll += tmp;
				}
            }
			tmpIdx -= tmpAll;
        }
		oneAns.append(tmpIdx + beg);
        ans.append(oneAns);
    }
}

double sqrt_2_PI = 2.5066282746310005024147107274575;
double normalDistribution(QPointF test, QPointF anchor, double r, double sigma) {
    QPointF diff = test - anchor;
    double x = qSqrt(diff.x() * diff.x() + diff.y() * diff.y()) - r;

    return 1./(sqrt_2_PI * sigma) * qExp(- x * x / (2. * sigma * sigma));
}
