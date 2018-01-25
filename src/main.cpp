#include "uiMainWindow.h"
#include <QApplication>
#include <QSettings>
#include "calcLibMatrixOp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    uiMainWindow w;
    w.show();

    return a.exec();
}
