#ifndef DATARECV_H
#define DATARECV_H
#include <QObject>
#include <QUdpSocket>
#include <QRegExp>
#include "dataDistanceLog.h"

class dataRecv : public QObject
{
    Q_OBJECT

public:
    dataRecv();

    void dataHandling(const QByteArray &dataIn);

private:
    QUdpSocket udpSocket;
    quint16 portNum;

    distanceData *q{nullptr};

    QRegExp rx;

private slots:
    void msgRecv();
};

#endif // DATARECV_H
