#include "showTagRelated.h"

QList<oneTagView> tagsView::viewDatabase;
int tagsView::count = 0;
QMap<int, oneTagView> showTagRelated::tagViewData;
tagsView showTagRelated::tagsViewDataBase;

tagsView::tagsView(){
    viewDatabase << oneTagView{{QColor{Qt::red},       QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::green},     QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::blue},      QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::cyan},      QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::magenta},   QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::yellow},    QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::darkRed},   QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::darkGreen}, QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::darkBlue},  QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::darkCyan},  QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    viewDatabase << oneTagView{{QColor{Qt::darkYellow},QColor{Qt::darkGray}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}};
    count = 0;
}

/******************************************************************/
showTagRelated::showTagRelated(){}
showTagRelated::showTagRelated(int tagId) {
    recordTagId(tagId);
}
void showTagRelated::recordTagId(int tagId) {
    if (!tagViewData.contains(tagId)) {
        tagViewData.insert(tagId, tagsViewDataBase.viewDatabase[tagsViewDataBase.count++]);
        this->tagId = tagId;
    }
    tagView = tagViewData[tagId];
}
void showTagRelated::addMethod(const QString &name) {
    if (!oneTagMethod.contains(name)) {
        showTagOneMethod tmp;
        tmp.name = name;
        tmp.posColorA = tagView.color[0];
        tmp.posColorB = tagView.color[1];
        tmp.linesColor = tagView.color[oneTagMethod.count()%2];
        oneTagMethod.insert(name, tmp);
    } else {
        qWarning() << "showTagRelated::addMethod $>" << name << "is Already exist";
    }
}

void showTagRelated::drawPoint(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColorA));
        painter.setPen  (QPen  (oneMethod.posColorB, 2));
        painter.drawEllipse(oneMethod.pos*ratio, shapeSize, shapeSize);
    }
}
void showTagRelated::drawPointsRaw(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColorA));
        painter.setPen  (QPen  (oneMethod.posColorB, 1));
        for (int j = 0; j < oneMethod.pointsRaw.count(); j++) {
            painter.drawEllipse(oneMethod.pointsRaw[j]*ratio, shapeSize-1, shapeSize-1);
        }
    }
}
void showTagRelated::drawPointsRefined(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setBrush(QBrush(oneMethod.posColorB));
        painter.setPen  (QPen  (Qt::black, 1));
        for (int j = 0; j < oneMethod.pointsRefined.count(); j++) {
            painter.drawEllipse(oneMethod.pointsRefined[j]*ratio, shapeSize-1, shapeSize-1);
        }
    }
}
void showTagRelated::drawLine(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        painter.setPen  (QPen(oneMethod.posColorA, 2));
        painter.drawLine(QLineF{oneMethod.line.p1()*ratio, oneMethod.line.p2()*ratio});
    }
}
void showTagRelated::drawLines(QPainter &painter, dType ratio) const {
    foreach (showTagOneMethod oneMethod, oneTagMethod) {
        qDebug() << "showTagRelated::drawLines $>" << oneMethod.linesColor;
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
