#include "uiUsrFrame.h"

const int uiUsrFrame::MAX_SHOWABLE_NUM = 6;
uiUsrFrame::uiUsrFrame(QWidget *parent) : QFrame(parent) {
    setStyleSheet("background-color:white;");
}

void uiUsrFrame::addOneUsr(int tagId, USR_STATUS status) {
    uiUsrInfoBtn *usrBtn = nullptr;
    if (MAX_SHOWABLE(nShowableBtns+1)) {
        usrBtn = new uiUsrInfoBtn(tagId, true, this);
        nShowableBtns ++;
    } else {
        usrBtn = new uiUsrInfoBtn(tagId, false, this);
    }
    qDebug() << "[@uiUsrFrame::addOneUsr] tagId:" << tagId << "nShowableBtns:" << nShowableBtns;
    usrBtn->show();
    usrBtn->setUsrStatus(status);

    int x = usrBtns.count() * usrBtn->width() % width();
    int y = usrBtns.count() * usrBtn->width() / width() * usrBtn->height();
    usrBtn->setGeometry(x, y, usrBtn->width(), usrBtn->height());

    usrBtns.append(usrBtn);
    connect(usrBtn, SIGNAL(oneUsrBtnClicked(int)), this, SLOT(oneUsrBtnClicked_slot(int)));

    update();
}

void uiUsrFrame::removeOneUsr(int tagId) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            delete usrBtns[i];
            usrBtns.removeAt(i);
        }
    }

    nShowableBtns --;
    update();
}

void uiUsrFrame::removeAll() {
    for (int i = 0; i < usrBtns.count(); i++) {
        delete usrBtns[i];
    }
    usrBtns.clear();

    nShowableBtns = 0;
    update();
}

void uiUsrFrame::setUsrStatus(int tagId, USR_STATUS status) {
    foreach (uiUsrInfoBtn *usrBtn, usrBtns) {
        if (usrBtn->getTagId() == tagId) {
            usrBtn->setUsrStatus(status);
        }
    }
}

bool uiUsrFrame::isShowable(int tagId) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            return usrBtns[i]->getShowable();
        }
    }
    return false;
}
QList<int> uiUsrFrame::getShowableTags() {
    QList<int> ans;
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getShowable()) {
            ans.append(usrBtns[i]->getTagId());
        }
    }
    return ans;
}

void uiUsrFrame::oneUsrBtnClicked_slot(int tagId) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            if (usrBtns[i]->getShowable()) {
                if (!MAX_SHOWABLE(nShowableBtns + 1)) {
                    qDebug() << "[@uiUsrFrame::oneUsrBtnClicked_slot] reject show, tagId:" << tagId;
                    usrBtns[i]->setShowable(false);
                } else {
                    nShowableBtns ++;
                    emit oneUsrBtnClicked_siganl(tagId, true);
                }
            } else {
                emit oneUsrBtnClicked_siganl(tagId, false);
                nShowableBtns --;
            }
        }
    }
}
