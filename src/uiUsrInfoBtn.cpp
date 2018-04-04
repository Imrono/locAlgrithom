#include "uiUsrInfoBtn.h"
#include <QPainter>
#include <QKeyEvent>
#include <QToolTip>
#include "calcLibGeometry.h"
#include "showTagDelegate.h"

uiUsrInfoBtn::uiUsrInfoBtn(int tagId, QWidget *parent) :
    tagId{tagId}, QToolButton(parent) {
    initial();
}

uiUsrInfoBtn::uiUsrInfoBtn(int tagId, bool isShowable, QWidget *parent) :
    tagId{tagId}, isShowable{isShowable}, QToolButton(parent) {
    initial();
}

void uiUsrInfoBtn::initial() {
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setFixedSize(40, 60);
    setIcon(QIcon(":/resource/usr/usr_A.png"));
    setIconSize(QSize(38, 38));

    QFont font;
    font.setBold(true);
    setFont(font);
    setText(QString("%0").arg(tagId, 4, 10, QChar('0')));

    syncShowable();

    connect(this, &QToolButton::clicked, this, [this](void) {
        setFocus();
        this->isShowable = !this->isShowable;
        syncShowable();
    });

    connect(this, &QToolButton::clicked, this, [this](void) {
        emit oneUsrBtnClicked(this->tagId);
    });

    contextMenu = new QMenu(this);
    showML_Action = new QAction(this);
    showML_Action->setText("show/hide LM");
    showDistInfo = new QAction(this);
    showDistInfo->setText("distance show");

    connect(showML_Action, &QAction::triggered, this, [this](void) {
        emit oneUsrShowML(this->tagId);
    });
    connect(showDistInfo, &QAction::triggered, this, [this](void) {
        emit oneUsrShowDistance(this->tagId);
    });
}

void uiUsrInfoBtn::setUsrStatus(USR_STATUS status) {
    this->status = status;
    switch (status) {
    case USR_STATUS::HAS_DISTANCE_DATA:
        setIcon(QIcon(":/resource/usr/usr_D.png"));
        break;
    case USR_STATUS::HAS_MEASURE_DATA:
        setIcon(QIcon(":/resource/usr/usr_C.png"));
        break;
    case USR_STATUS::HAS_TRACK_DATA:
        setIcon(QIcon(":/resource/usr/usr_B.png"));
        break;
    default:
        setIcon(QIcon(":/resource/usr/usr_A.png"));
        break;
    }
}

void uiUsrInfoBtn::syncShowable() {
    if (isShowable) {
        setStyleSheet("QToolButton{background-color:GhostWhite;}");
    } else {
        isShowToolTip = false;
        setStyleSheet("QToolButton{background-color:DimGray;}");
        setToolTip("");
    }
}

void uiUsrInfoBtn::paintEvent(QPaintEvent *event) {
    QToolButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(colorSample));
    if (isShowable) {
        if (isShowingDist) {
            showTagDelegate::draw5Star(painter, QPointF{5.f, 5.f}, colorSample, 5.f, 0.f);
        } else {
            painter.drawEllipse(QPointF{5.f, 5.f}, 3, 3);
        }
    }
}

void uiUsrInfoBtn::keyPressEvent(QKeyEvent *e) {
    switch(e->key())
    {
    case Qt::Key_Enter:
	case Qt::Key_Return:
        emit clicked();
        break;
    default:
        break;
    }
    QToolButton::keyPressEvent(e);
}

void uiUsrInfoBtn::contextMenuEvent(QContextMenuEvent *e) {
    if (isShowable
    && status != USR_STATUS::HAS_DISTANCE_DATA) {
        setStyleSheet("QMenu{color: black;}"
                      "QMenu::item:selected{background-color: lightgray;}");
        contextMenu->clear();
        contextMenu->addAction(showML_Action);
        contextMenu->addAction(showDistInfo);

        contextMenu->exec(QCursor::pos());
    } else {}
    e->accept();
}

void uiUsrInfoBtn::setShowToolTip(bool isShow,
                                  const int *distance,
                                  const dType * weight,
                                  const locationCoor *sensor,
                                  const int N,
                                  QPointF real, QPointF canvas) {
    isShowToolTip = isShow;
    posReal = real;
    posCanvas = canvas;
    if (isShowToolTip) {
        QString toolTip = QString("real:(%1,%2)\nshow:(%3,%4)\n").arg(posReal.x()).arg(posReal.y())
                .arg(posCanvas.x()).arg(posCanvas.y());
        if (distance && weight && sensor && N > 0) {
            toolTip += QString("distance | weight | diffDist (%1)\n").arg(N);
            for(int i = 0; i < N; i++) {
                toolTip += QString("%1 | %2 | %3\n")
                        .arg(distance[i], 8)
                        .arg(QString::number(weight[i]).left(5), 5, QChar(' '))
                        .arg(calcDistance(real, sensor[i].toQPointF()) - distance[i]);
            }
        }

        setToolTip(toolTip);
        setToolTipDuration((int)(((unsigned int)(-1)) >> 1));
    } else {
        setToolTip("");
        setToolTipDuration(0);
    }
}
