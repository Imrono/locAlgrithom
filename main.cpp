﻿#include "uiMainWindow.h"
#include <QApplication>
#include <QSettings>
#include "datatype.h"
#include "_myheads.h"
#include <calcTagPos.h>
#include "showStore.h"
#include "calcTargetTracking.h"
#include "dataSensorIni.h"
#include "dataDistanceLog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    uiMainWindow w;
    w.show();

    return a.exec();
}
