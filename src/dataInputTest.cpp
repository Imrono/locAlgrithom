#include "dataInputTest.h"

dataInputTest::dataInputTest()
{

}

void dataInputTest::initFakeData() {
    oneLogData tmpData;
    tmpData.distance = QVector<int>(6, 0);

    oneDistData = tmpData;
}
