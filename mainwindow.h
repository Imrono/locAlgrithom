#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include "datatype.h"
#include "calcPos.h"
#include "showPoint.h"
#include "showStore.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(showStore *store, QWidget *parent = 0);
    ~MainWindow();

    showStore *store;

private:
    Ui::MainWindow *ui;
    QTimer timer;

    int distCount{1};

    void paintEvent(QPaintEvent *event);

private slots:
    void handleTimeout();
};

#endif // MAINWINDOW_H
