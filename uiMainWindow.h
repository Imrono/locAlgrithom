#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "_myheads.h"
#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include "datatype.h"
#include "dataSensorIni.h"
#include "calcPos.h"
#include "showTagRelated.h"
#include "showStore.h"

namespace Ui {
class MainWindow;
}

class uiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit uiMainWindow(showStore *store, QWidget *parent = 0);
    ~uiMainWindow();

    void setConfigData(const configData *d);

    showStore *store;

signals:
    void countChanged(int cnt);

private:
    Ui::MainWindow *ui;
    QTimer timer;
    bool timerStarted;

    int distCount{0};

    void paintEvent(QPaintEvent *event);

private slots:
    void handleTimeout();
};

#endif // MAINWINDOW_H
