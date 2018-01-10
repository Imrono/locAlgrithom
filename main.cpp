#include "uiMainWindow.h"
#include <QApplication>
#include <QSettings>
#include "dataType.h"
#include "_myheads.h"
#include <calcTagPos.h>
#include "showStore.h"
#include "calcTagTrack.h"
#include "dataSensorIni.h"
#include "dataDistanceLog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    uiMainWindow w;
    w.show();

    return a.exec();
}
