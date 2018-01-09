#include "uiUsrInfoBtn.h"
#include <QPainter>

uiUsrInfoBtn::uiUsrInfoBtn(int tagId, QWidget *parent) :
    tagId{tagId}, QToolButton(parent) {
    initial();

    connect(this, &QToolButton::clicked, this, [this](void) {
        this->isShowable = !this->isShowable;
        syncShowable();
    });

    connect(this, &QToolButton::clicked, this, [this](void) {
        emit oneUsrBtnClicked(this->tagId);
    });
}

uiUsrInfoBtn::uiUsrInfoBtn(int tagId, bool isShowable, QWidget *parent) :
    tagId{tagId}, isShowable{isShowable}, QToolButton(parent) {
    initial();

    connect(this, &QToolButton::clicked, this, [this](void) {
        this->isShowable = !this->isShowable;
        syncShowable();
    });

    connect(this, &QToolButton::clicked, this, [this](void) {
        emit oneUsrBtnClicked(this->tagId);
    });
}

void uiUsrInfoBtn::initial() {
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setFixedSize(40, 60);
    setIcon(QIcon(":/usr/resource/usr/usr_A.png"));
    setIconSize(QSize(38, 38));

    QFont font;
    font.setBold(true);
    setFont(font);
    setText(QString("%0").arg(tagId, 4, 10, QChar('0')));

    syncShowable();
}

void uiUsrInfoBtn::setUsrStatus(USR_STATUS status) {
    switch (status) {
    case USR_STATUS::HAS_DISTANCE_DATA:
        setIcon(QIcon(":/usr/resource/usr/usr_D.png"));
        break;
    case USR_STATUS::HAS_MEASURE_DATA:
        setIcon(QIcon(":/usr/resource/usr/usr_C.png"));
        break;
    case USR_STATUS::HAS_TRACK_DATA:
        setIcon(QIcon(":/usr/resource/usr/usr_B.png"));
        break;
    default:
        setIcon(QIcon(":/usr/resource/usr/usr_A.png"));
        break;
    }
}

void uiUsrInfoBtn::syncShowable() {
    if (isShowable) {
        setStyleSheet("background-color:GhostWhite;");
    } else {
        setStyleSheet("background-color:DimGray;");
    }
}

void uiUsrInfoBtn::paintEvent(QPaintEvent *event) {
    QToolButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(colorA));
    if (isShowable) {
        painter.drawEllipse(QPointF{5.f, 5.f}, 3, 3);
    }
}
