#include "showStore.h"

showStore::showStore(locationCoor loc[4])
{
    this->loc[0] = loc[0];
    this->loc[1] = loc[1];
    this->loc[2] = loc[2];
    this->loc[3] = loc[3];
}

void showStore::appendLabel(const QString &name) {
    labelInfo *label = new labelInfo;
    label->name = name;
    if (!name2idx.contains(name)) {
        name2idx.insert(name, labels.count());
    }
    labels.append(label);
}
void showStore::appendLabel(const QString &name, showPoint showStyle) {
    labelInfo *label = new labelInfo;
    label->name = name;
    label->showStyle = showStyle;
    if (!name2idx.contains(name)) {
        name2idx.insert(name, labels.count());
    }
    labels.append(label);
}
void showStore::appendLabel(labelInfo *label) {
    if (!name2idx.contains(label->name)) {
        name2idx.insert(label->name, labels.count());
    }
    labels.append(label);
}
labelInfo *showStore::getLabel(int idx) {
    return labels[idx];
}

void showStore::setName(int idx, const QString &name) {
    if (idx >= 0 && idx < labels.count())
        labels[idx]->name = name;
}
void showStore::setShowStyle(int idx, showPoint showStyle) {
    if (idx >= 0 && idx < labels.count())
        labels[idx]->showStyle = showStyle;
}

void showStore::addRawPoints(int idx, QVector<locationCoor> points) {
    if (idx >= 0 && idx < labels.count())
        labels[idx]->RawPoints.append(points);
}

void showStore::clearRawPoints(int idx) {
    if (idx < labels.count())
        labels[idx]->RawPoints.clear();
}

void showStore::addAnsPoint(int idx, locationCoor p) {
    if (idx < labels.count())
        labels[idx]->Ans.append(p);
}
QVector<locationCoor> showStore::getAnsPointVector(int idx) {
    if (idx >= 0 && idx < labels.count())
        return labels[idx]->Ans;
    else
        return QVector<locationCoor>();
}
locationCoor showStore::getAnsPoint(int idx, int pointIdx) {
    if (idx < labels.count())
        return labels[idx]->Ans[pointIdx];
    else
        return {0, 0, 0};
}
void showStore::clearAnsPoints(int idx) {
    if (idx < labels.count())
        labels[idx]->Ans.clear();
}
