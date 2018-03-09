#include "calcLibMath_TEST.h"

calcLibMath_TEST::calcLibMath_TEST() {

}

void calcLibMath_TEST::randomDraw_TEST() {
    QVector<QVector<int>> ans;
    int d[15][4] = {
        {0,1,2,3},  //0
        {0,1,2,4},
        {0,1,2,5},
        {0,1,3,4},
        {0,1,3,5},
        {0,1,4,5},
        {0,2,3,4},
        {0,2,3,5},
        {0,2,4,5},
        {0,3,4,5},
        {1,2,3,4},
        {1,2,3,5},
        {1,2,4,5},
        {1,3,4,5},
        {2,3,4,5}   //14  ==> total: 15
    };
    bool judge[15] = {true,true,true,true,true,true,true,true,
                      true,true,true,true,true,true,true};

    randomDraw(ans, 4, 6, 10);
    qDebug() << ans;

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 10; j++) {
            if (d[i][0] == ans[j][0]
             && d[i][1] == ans[j][1]
             && d[i][2] == ans[j][2]
             && d[i][3] == ans[j][3]) {
                judge[i] = false;
            }
        }
    }

    int count = 0;
    for (int i = 0; i < 15; i++) {
        if (false == judge[i]) {
            count ++;
        }
    }
    QVERIFY(10 == count);
}
