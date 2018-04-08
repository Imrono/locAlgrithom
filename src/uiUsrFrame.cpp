#include "uiUsrFrame.h"

const int uiUsrFrame::MAX_SHOWABLE_NUM = 6;
uiUsrFrame::uiUsrFrame(QWidget *parent) : QFrame(parent) {
    setStyleSheet("background-color:white;");
    setGeometry(0, 0, 160, 180);
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
    connect(usrBtn, SIGNAL(oneUsrShowDistance(int)), this, SIGNAL(oneUsrShowDistance_siganl(int)));

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

void uiUsrFrame::setEnabledAll(bool enable) {
    foreach (uiUsrInfoBtn *btn, usrBtns) {
        btn->setEnabled(enable);
    }
}

// HAS_DISTANCE_DATA, HAS_MEASURE_DATA, HAS_TRACK_DATA
void uiUsrFrame::setUsrStatus(int tagId, USR_STATUS status) {
    foreach (uiUsrInfoBtn *usrBtn, usrBtns) {
        if (usrBtn->getTagId() == tagId) {
            usrBtn->setUsrStatus(status);
        }
    }
}
USR_STATUS uiUsrFrame::getUsrStatus(int tagId) {
    USR_STATUS usrStatus;
    foreach (uiUsrInfoBtn *usrBtn, usrBtns) {
        if (usrBtn->getTagId() == tagId) {
            usrStatus = usrBtn->getUsrStatus();
        }
    }
    return usrStatus;
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

void uiUsrFrame::setBtnColorSample(int tagId, const QColor &color) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            usrBtns[i]->setColorSample(color);
        }
    }
}
QColor uiUsrFrame::getBtnColorSample(int tagId) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            return usrBtns[i]->getColorSample();
        }
    }
    return QColor();
}

void uiUsrFrame::oneUsrBtnClicked_slot(int tagId) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            // if not enabled, try to enable it
            if (usrBtns[i]->getShowable()) {    // isShowable is changed during btn click
                // pool is full, discard the command
                if (!MAX_SHOWABLE(nShowableBtns + 1)) {
                    qDebug() << "[@uiUsrFrame::oneUsrBtnClicked_slot] reject show, tagId:" << tagId;
                    usrBtns[i]->setShowable(false);
                // pool is not full, enable the button
                } else {
                    // max likehood show model, disable old tag and enable this tag
                    if (-1 != tagShowLM && tagId != tagShowLM) {
                        for (int j = 0; j < usrBtns.count(); j++) {
                            if(usrBtns[j]->getTagId() == tagShowLM) {
                                usrBtns[j]->setShowable(false);
                                emit oneUsrBtnClicked_siganl(tagShowLM, false);
                                nShowableBtns --;
                                break;
                            }
                        }
                        tagShowLM = tagId;
                    // normal show model, do nothing
                    } else {}

                    nShowableBtns ++;
                    emit oneUsrBtnClicked_siganl(tagId, true);
                }
            // if enabled, disable it
            } else {
                emit oneUsrBtnClicked_siganl(tagId, false);
                nShowableBtns --;
            }
        }
    }
}

void uiUsrFrame::oneUsrShowML_slot(int tagId) {
    if (tagShowLM == tagId) {
        tagShowLM = UN_INIT_LM_TAGID;
    } else {
        tagShowLM = tagId;
    }

    // disable all tags without this tagId
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            if (!usrBtns[i]->getShowable()) {
                usrBtns[i]->setShowable(true);
                nShowableBtns ++;
                emit oneUsrBtnClicked_siganl(tagId, true);
            }
        } else {
            if (usrBtns[i]->getShowable()) {
                usrBtns[i]->setShowable(false);
                emit oneUsrBtnClicked_siganl(tagId, false);
                nShowableBtns --;
            }
        }
    }
    // notice the ui to check to Max Likehood model
    emit oneUsrShowML_siganl(tagId, UN_INIT_LM_TAGID != tagShowLM);
    emit oneUsrShowDistance_siganl(tagId);
}

void uiUsrFrame::setShowDistTagId(int tagId) {
    if (showDistTagId != tagId) {
        for (int i = 0; i < usrBtns.count(); i++) {
            if (usrBtns[i]->getTagId() == tagId) {
                usrBtns[i]->setIsShowingDist(true);
            } else if (usrBtns[i]->getTagId() == showDistTagId) {
                usrBtns[i]->setIsShowingDist(false);
            }
        }
        showDistTagId = tagId;
    } else {}
}

void uiUsrFrame::setBtnToolTip(int tagId, bool isShowPos,
                               const int *distance,
                               const dType * weight,
                               const locationCoor *sensor,
                               const int N,
                               QPointF real, QPointF canvas) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            usrBtns[i]->setShowToolTip(isShowPos, distance, weight, sensor, N, real, canvas);
        }
    }
}

void uiUsrFrame::setBtnEnableLM(int tagId, bool enable) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            usrBtns[i]->setBtnEnableLM(enable);
        }
    }
}

void uiUsrFrame::setChartData(int tagId, const QString name,
                              const QVector<qreal> &v, const QVector<qreal> &a) {
    for (int i = 0; i < usrBtns.count(); i++) {
        if (usrBtns[i]->getTagId() == tagId) {
            usrBtns[i]->setChartData(name, v, a);
        }
    }
}
