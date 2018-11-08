#ifndef DATAINPUTTEST_H
#define DATAINPUTTEST_H
#include "dataInputBase.h"
#include "_myheads.h"
#include <QObject>

class dataInputTest : public dataInputBase
{
    Q_OBJECT

public:
    dataInputTest();

    void initFakeData();

    oneLogData oneDistData;
};

#endif // DATAINPUTTEST_H
