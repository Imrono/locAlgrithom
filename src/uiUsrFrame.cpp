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
    connect(usrBtn, SIGNAL(oneUsrShowML(int)), this, SLOT(oneUsrShowML_slot(int)));

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
    tagShowLM = -1;
	emit oneUsrShowML_siganl(tagShowLM, false);

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

void uiUsrFrame::setBtnColorA(int tagId, const QColor &color) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            usrBtns[i]->setColorA(color);
        }
    }
}

void uiUsrFrame::clrBtnColorA(int tagId) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            usrBtns[i]->clrColorA();
        }
    }
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

void uiUsrFrame::oneUsrShowML_slot(int tagId) {
    if (tagShowLM == tagId) {
        tagShowLM = -1;
    } else {
        tagShowLM = tagId;
    }

    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() != tagId) {
            if (usrBtns[i]->getShowable()) {    //disable all usr's show
                usrBtns[i]->setShowable(false);
                emit oneUsrBtnClicked_siganl(tagId, false);
                nShowableBtns --;
            } else {}
        }
    }

    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            if (!usrBtns[i]->getShowable()) {
                usrBtns[i]->setShowable(true);
                nShowableBtns ++;
            }
            emit oneUsrBtnClicked_siganl(tagId, true);
		} else {
			if (usrBtns[i]->getShowable()) {
				usrBtns[i]->setShowable(false);
                nShowableBtns --;
			}
			emit oneUsrBtnClicked_siganl(tagId, false);
		}
    }

    emit oneUsrShowML_siganl(tagId, -1 != tagShowLM);
}
