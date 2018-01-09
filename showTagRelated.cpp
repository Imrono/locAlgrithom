#include "showTagRelated.h"

QList<oneTagView> tagsView::viewDatabase;
int tagsView::count = 0;
QMap<int, oneTagView> showTagRelated::tagViewData;
tagsView showTagRelated::tagsViewDataBase;

tagsView::tagsView(){
    viewDatabase << oneTagView{{QColor{Qt::red},    QColor{Qt::darkRed}},    {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 0, false};
    viewDatabase << oneTagView{{QColor{Qt::green},  QColor{Qt::darkGreen}},  {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 1, false};
    viewDatabase << oneTagView{{QColor{Qt::blue},   QColor{Qt::darkBlue}},   {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 2, false};
    viewDatabase << oneTagView{{QColor{Qt::cyan},   QColor{Qt::darkCyan}},   {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 3, false};
    viewDatabase << oneTagView{{QColor{Qt::magenta},QColor{Qt::darkMagenta}},{SHOW_SHAPE::radius, SHOW_SHAPE::square}, 4, false};
    viewDatabase << oneTagView{{QColor{Qt::yellow}, QColor{Qt::darkYellow}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 5, false};
    count = 0;
}

/******************************************************************/
showTagRelated::showTagRelated(){}
showTagRelated::showTagRelated(int tagId) : tagId{tagId} {
    recordTagId(tagId);
    tagView = tagViewData[tagId];
    qDebug() << "[@showTagRelated::showTagRelated] tagId:" << tagId << tagView.color[0] << tagView.color[1] << tagView.isUsed;
}
void showTagRelated::recordTagId(int tagId) {
    if (!tagViewData.contains(tagId)) {
        for (int i = 0; i < tagsViewDataBase.viewDatabase.count(); i++) {
            if (false == tagsViewDataBase.viewDatabase[i].isUsed) {
                tagsViewDataBase.viewDatabase[i].isUsed = true;
                tagViewData.insert(tagId, tagsViewDataBase.viewDatabase[i]);
                tagsViewDataBase.count++;
                break;
            }
        }
    }
}
void showTagRelated::eraseTagId(int tagId) {
    if (tagViewData.contains(tagId)) {
        tagsViewDataBase.viewDatabase[tagViewData[tagId].nColorStyle].isUsed = false;
        tagViewData.remove(tagId);
        tagsViewDataBase.count--;
    }
}

void showTagRelated::addMethod(const QString &name) {
    if (!oneTagMethod.contains(name)) {
        showTagOneMethod tmp;
        tmp.name = name;
        tmp.posColor = tagView.color[oneTagMethod.count()%2];
        tmp.linesColor = tagView.color[oneTagMethod.count()%2];
        oneTagMethod.insert(name, tmp);
    } else {
        qWarning() << "showTagRelated::addMethod $>" << name << "is Already exist";
    }
}
void showTagRelated::setTagView() {
    if (tagViewData.contains(tagId)) {
        tagView = tagViewData[tagId];
    }
}

void showTagRelated::drawPoint(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColor));
        painter.setPen  (QPen  (QColor(Qt::darkGray), 2));
        painter.drawEllipse(oneMethod.pos*ratio, shapeSize, shapeSize);
    }
}
void showTagRelated::drawPointsRaw(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColor));
        painter.setPen  (QPen  (QColor(Qt::darkGray), 1));
        for (int j = 0; j < oneMethod.pointsRaw.count(); j++) {
            painter.drawEllipse(oneMethod.pointsRaw[j]*ratio, shapeSize-1, shapeSize-1);
        }
    }
}
void showTagRelated::drawPointsRefined(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(QColor(Qt::darkGray)));
        painter.setPen  (QPen  (Qt::black, 1));
        for (int j = 0; j < oneMethod.pointsRefined.count(); j++) {
            painter.drawEllipse(oneMethod.pointsRefined[j]*ratio, shapeSize-1, shapeSize-1);
        }
    }
}
void showTagRelated::drawLine(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setPen  (QPen(oneMethod.posColor, 2));
        painter.drawLine(QLineF{oneMethod.line.p1()*ratio, oneMethod.line.p2()*ratio});
    }
}
void showTagRelated::drawLines(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setPen(QPen(oneMethod.linesColor, 2));
        for (int j = 0; j < oneMethod.lines.count(); j++)
            painter.drawLine(QLineF{oneMethod.lines[j].p1()*ratio,
                                    oneMethod.lines[j].p2()*ratio});
    }
}

void showTagRelated::drawCircle(QPainter &painter, const QVector<locationCoor> &sensor, dType ratio) const {
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    for (int i = 0; i < distance.count(); i++) {
        painter.drawEllipse(sensor[i].toQPointF() * ratio, distance[i] * ratio, distance[i] * ratio);
    }
}

void showTagRelated::drawCircleBold(QPainter &painter, const locationCoor &sensor,
                                    int distIdx, dType ratio) const {
    painter.setPen(QPen(Qt::black, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(sensor.toQPointF() * ratio, distance[distIdx] * ratio, distance[distIdx] * ratio);
}
