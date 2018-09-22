#include "uiUsrInfoBtn.h"
#include <QPainter>
#include <QKeyEvent>
#include <QToolTip>
#include "calcLibGeometry.h"
#include "showTagDelegate.h"

#include <QtCharts/QChartView>

uiUsrInfoBtn::uiUsrInfoBtn(int tagId, QWidget *parent) :
    tagId{tagId}, QToolButton(parent) {
    initial();
}

uiUsrInfoBtn::uiUsrInfoBtn(int tagId, bool isShowable, QWidget *parent) :
    tagId{tagId}, isShowable{isShowable}, QToolButton(parent) {
    initial();
}

uiUsrInfoBtn::~uiUsrInfoBtn() {
    if (toolTipWidget) delete toolTipWidget;
}

void uiUsrInfoBtn::initial() {
    setMouseTracking(true);
    toolTipWidget = new uiUsrTooltip();
    toolTipWidget->hide();

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
        if (mouseAt)
            if (toolTipWidget->isHidden())
                toolTipWidget->show();
    } else {
        setStyleSheet("QToolButton{background-color:DimGray;}");
        if (mouseAt)
            toolTipWidget->hide();
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
        if (isEnableLM) {
            contextMenu->addAction(showML_Action);
        }
        contextMenu->addAction(showDistInfo);

        contextMenu->exec(QCursor::pos());
    } else {}
    e->accept();
}
void uiUsrInfoBtn::mouseMoveEvent(QMouseEvent *e) {
    if (isShowable && status > USR_STATUS::HAS_DISTANCE_DATA) {
        toolTipWidget->move(mapToGlobal(e->pos() + QPoint(10, -150)));
        if (toolTipWidget->isHidden()) {
            toolTipWidget->show();
        }
    }
}

void uiUsrInfoBtn::enterEvent(QEvent *e) {
    Q_UNUSED(e);
    mouseAt = true;
}
void uiUsrInfoBtn::leaveEvent(QEvent *e) {
    Q_UNUSED(e);
    toolTipWidget->hide();
    mouseAt = false;
}

void uiUsrInfoBtn::setShowToolTip(bool isShow,
                                  const int *distance,
                                  const dType * weight,
                                  const locationCoor *sensor,
                                  const int N,
                                  QPointF real, QPointF canvas) {
    tooltipInfo info;
    info.tagId = tagId;
    info.isShow = isShow;

    if (isShow) {
        for (int i{0}; i < N; i++) {
            info.distance.append(distance[i]);
            info.weight.append(weight[i]);
            info.diff.append(calcDistance(real, sensor[i].toQPointF()) - distance[i]);
        }
        info.pos = real;
        info.track = canvas;
    }
    toolTipWidget->fillToolTipUI(info);
}

void uiUsrInfoBtn::setChartData(const QString &name,
                                const QVector<qreal> &v, const QVector<qreal> &a) {
    toolTipWidget->setChartData(name, v, a);
}
