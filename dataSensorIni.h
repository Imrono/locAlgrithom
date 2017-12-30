#ifndef DATASENSORINI_H
#define DATASENSORINI_H
#include "_myheads.h"
#include <QSettings>
#include <QVector>
#include "datatype.h"

#define my2Str(xyz, idx)    QString("%0%1").arg(xyz).arg(idx)

struct configData {
    QVector<locationCoor> sensor;
    QVector<QVector<locationCoor>> stop;
    QVector<QVector<locationCoor>> alarm;
    QVector<QVector<locationCoor>> oper;

    void reset() {
        sensor.clear();
        stop.clear();
        alarm.clear();
        oper.clear();
    }

    QString toString();
};

class dataSensorIni
{
public:
    explicit dataSensorIni();
    ~dataSensorIni();
    void loadNewFile(const QString &fileName);

    QString toString();
    configData *get_q() const {
        return q;
    }

private:
    QString fileName;

    configData *q{nullptr};
};

#endif // DATASENSORINI_H
