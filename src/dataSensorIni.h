#ifndef DATASENSORINI_H
#define DATASENSORINI_H
#include "_myheads.h"
#include <QSettings>
#include <QVector>
#include "dataType.h"

#define my2Str(xyz, idx)    QString("%0%1").arg(xyz).arg(idx)

struct configData {
    QVector<locationCoor> sensor;
    QVector<QVector<locationCoor>> stop;
    QVector<QVector<locationCoor>> alarm;
    QVector<QVector<locationCoor>> oper;
    bool isInitialized{false};

    void clear() {
        sensor.clear();
        stop.clear();
        alarm.clear();
        oper.clear();
        isInitialized = false;
    }

    QString toString();
};

class dataSensorIni
{
public:
    explicit dataSensorIni();
    ~dataSensorIni();
    void loadNewFile(const QString &fileName);
    void analyzeCfgData(QSettings &iniSetting);

    QString toString();
    configData *get_q() const {
        return q;
    }

private:
    QString fileName;

    configData *q{nullptr};
};

#endif // DATASENSORINI_H
