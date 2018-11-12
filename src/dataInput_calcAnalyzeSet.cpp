#include "dataInput_calcAnalyzeSet.h"
#include "uiMainWindow.h"
#include "ui_mainwindow.h"

dataInput_calcAnalyzeSet::dataInput_calcAnalyzeSet(int type, uiMainWindow *win) : usrFrame(win->ui->UsrFrm)
{
    if (uiMainWindow::CALC_LOG_ANALYZE == type) {
        dataInputHandler = new dataInputLog();
        dataInputType = uiMainWindow::CALC_LOG_ANALYZE;
        isActive = false;
        strName = "CALC_LOG_ANALYZE";
    } else
    if (uiMainWindow::CALC_TEST_ANALYZE == type) {
        dataInputHandler = new dataInputTest();
        dataInputType = uiMainWindow::CALC_TEST_ANALYZE;
        isActive = false;
        strName = "CALC_TEST_ANALYZE";
    } else
    if (uiMainWindow::CALC_CAN_ANALYZE == type) {
        dataInputHandler = new dataInputCan();
        dataInputType = uiMainWindow::CALC_CAN_ANALYZE;
        isActive = false;
        strName = "CALC_CAN_ANALYZE";
    } else
    if (uiMainWindow::CALC_POS_ANALYZE == type) {
        dataInputHandler = new dataInputPos();
        dataInputType = uiMainWindow::CALC_POS_ANALYZE;
        isActive = false;
        strName = "CALC_POS_ANALYZE";
    } else {}
    dataInputHandler->setMainWin(win);
}

void dataInput_calcAnalyzeSet::handleModelChange(bool isSelected, uiMainWindow *win){
    if (!isSelected) {
        usrFrame.hide();
        usrFrame.removeAll();
        modelStore.removeAll();
        modelStore.calcPosType = CALC_POS_TYPE::POS_NONE;
        modelStore.calcTrackMethod = TRACK_METHOD::TRACK_NONE;
    } else {
        win->ui->showPath->setEnabled(true);
        win->ui->allPos->setEnabled(true);
        win->ui->showRadius->setEnabled(true);
        win->ui->showTrace->setEnabled(true);
        win->ui->showCross->setEnabled(true);
        win->ui->btn_drawTagId->setEnabled(true);
        win->ui->distCountEdit->setEnabled(true);
        win->ui->beginTrack->setEnabled(true);
        win->ui->gotoCount->setEnabled(true);
        win->ui->previous->setEnabled(true);
        win->ui->next->setEnabled(true);
        win->ui->reset->setEnabled(true);
        win->ui->refresh->setEnabled(true);
        win->ui->sigmaSlider->setEnabled(true);
        win->setStatusTimeInfo(false);
    }

    if (uiMainWindow::CALC_LOG_ANALYZE == dataInputType) {
        handleModelChangeLog(isSelected, win);
    } else
    if (uiMainWindow::CALC_TEST_ANALYZE == dataInputType) {
        handleModelChangeTest(isSelected, win);
    } else
    if (uiMainWindow::CALC_CAN_ANALYZE == dataInputType) {
        handleModelChangeCan(isSelected, win);
    } else
    if (uiMainWindow::CALC_POS_ANALYZE == dataInputType) {
        handleModelChangePos(isSelected, win);
    } else {}
}

void dataInput_calcAnalyzeSet::handleModelChangeLog(bool isSelected, uiMainWindow *win) {
    qDebug() << "handleModelChangeLog from" << isActive << "to" << isSelected;
    if (isActive != isSelected) {
        isActive = isSelected;

        if (isActive) {
            QMap<int, oneTag> &tagsData = dynamic_cast<dataInputLog *>(dataInputHandler)->get_q()->tagsData;
            if (0 != tagsData.count()) {
                foreach (const oneTag &tag, tagsData) {
                    modelStore.addNewTagInfo(tag.tagId);
                    usrFrame.setBtnEnableLM(tag.tagId, false);
                    usrFrame.addOneUsr(tag.tagId, USR_STATUS::HAS_DISTANCE_DATA);
                }
                for (int i = 0; i < 32 - modelStore.tags.count(); i++) {
                    usrFrame.addOneUsr(0, USR_STATUS::HAS_NONE_DATA);
                }
            }

            const QVector<int> distance(6, 0);
            switch (win->cfgData.get_q()->sensor.count()) {
            case 6: IS_TEST_CHANGE_DISTANCE(5, win->ui, false);
            case 5: IS_TEST_CHANGE_DISTANCE(4, win->ui, false);
            case 4: IS_TEST_CHANGE_DISTANCE(3, win->ui, false);
            case 3: IS_TEST_CHANGE_DISTANCE(2, win->ui, false);
            case 2: IS_TEST_CHANGE_DISTANCE(1, win->ui, false);
            case 1: IS_TEST_CHANGE_DISTANCE(0, win->ui, false);
            default:
                RESET_SHOW_DIST_DATA(p, win->ui);
            }

            usrFrame.show();
            win->ui->canvas->syncWithUiFrame(&usrFrame);
        } else {}
    }
}

void dataInput_calcAnalyzeSet::handleModelChangeTest(bool isSelected, uiMainWindow *win) {
    qDebug() << "handleModelChangeTest from" << isActive << "to" << isSelected;
    if (isActive != isSelected) {
        isActive = isSelected;
        win->ui->actionKalmanInfoTrack->setEnabled(!isActive);
        win->ui->actionKalmanLiteTrack->setEnabled(!isActive);
        win->ui->actionKalmanTrack->setEnabled(!isActive);
        win->ui->actionKalmanCoupled->setEnabled(!isActive);

        if (isActive) {
            win->ui->showPath->setEnabled(true);
            win->ui->allPos->setEnabled(true);
            win->ui->showRadius->setEnabled(true);
            win->ui->showTrace->setEnabled(true);
            win->ui->showCross->setEnabled(true);
            win->ui->btn_drawTagId->setEnabled(true);

            static_cast<dataInputTest *>(dataInputHandler)->initFakeData();
            modelStore.addNewTagInfo(TEST_TAG_ID);
            usrFrame.setBtnEnableLM(TEST_TAG_ID, false);
            usrFrame.addOneUsr(TEST_TAG_ID, USR_STATUS::HAS_DISTANCE_DATA);
            usrFrame.setShowDistTagId(TEST_TAG_ID);
            win->distanceShowTagId = TEST_TAG_ID;   //change showDist data
            usrFrame.update();

            win->ui->label_Id->setText("<b><font black>----<\font><\b>");
            const QVector<int> distance(6, 0);
            switch (win->cfgData.get_q()->sensor.count()) {
            case 6: IS_TEST_CHANGE_DISTANCE(5, win->ui, true);
            case 5: IS_TEST_CHANGE_DISTANCE(4, win->ui, true);
            case 4: IS_TEST_CHANGE_DISTANCE(3, win->ui, true);
            case 3: IS_TEST_CHANGE_DISTANCE(2, win->ui, true);
            case 2: IS_TEST_CHANGE_DISTANCE(1, win->ui, true);
            case 1: IS_TEST_CHANGE_DISTANCE(0, win->ui, true);
            default:
                RESET_SHOW_DIST_DATA(p, win->ui);
            }

            usrFrame.show();
            win->ui->canvas->syncWithUiFrame(&usrFrame);

            int tagShowLM = usrFrame.getTagShowLM();
            win->oneUsrShowML(tagShowLM, tagShowLM != UN_INIT_LM_TAGID);

            win->setStatusTimeInfo();
            win->setStatusIter(-1, -1, -1, -1);
            win->handleModelDataUpdate(false);
        } else {}
    }
}

void dataInput_calcAnalyzeSet::handleModelChangeCan(bool isSelected, uiMainWindow *win) {
    qDebug() << "handleModelChangeCan from" << isActive << "to" << isSelected;
    if (isActive != isSelected) {
        isActive = isSelected;

        if (isActive) {
            win->ui->refresh->setEnabled(false);

            usrFrame.show();
            win->ui->canvas->syncWithUiFrame(&usrFrame);

            static_cast<dataInputCan *>(dataInputHandler)->startDistanceFetchTimer();
        } else {
            static_cast<dataInputCan *>(dataInputHandler)->stopDistanceFetchTimer();
        }
    }
}

void dataInput_calcAnalyzeSet::handleModelChangePos(bool isSelected, uiMainWindow *win) {
    qDebug() << "handleModelChangePos from" << isActive << "to" << isSelected;
    if (isActive != isSelected) {
        isActive = isSelected;

        if (isActive) {
            win->ui->allPos->setEnabled(false);
            win->ui->showRadius->setEnabled(false);
            win->ui->showTrace->setEnabled(false);
            win->ui->showCross->setEnabled(false);
            win->ui->distCountEdit->setEnabled(false);
            win->ui->beginTrack->setEnabled(false);
            win->ui->gotoCount->setEnabled(false);
            win->ui->previous->setEnabled(false);
            win->ui->next->setEnabled(false);
            win->ui->reset->setEnabled(false);
            win->ui->refresh->setEnabled(false);
            win->ui->sigmaSlider->setEnabled(false);
            win->setStatusTimeInfo(true);

            win->ui->label_Id->setText("<b><font black>----<\font><\b>");
            const QVector<int> distance(6, 0);
            switch (win->cfgData.get_q()->sensor.count()) {
            case 6: IS_TEST_CHANGE_DISTANCE(5, win->ui, false);
            case 5: IS_TEST_CHANGE_DISTANCE(4, win->ui, false);
            case 4: IS_TEST_CHANGE_DISTANCE(3, win->ui, false);
            case 3: IS_TEST_CHANGE_DISTANCE(2, win->ui, false);
            case 2: IS_TEST_CHANGE_DISTANCE(1, win->ui, false);
            case 1: IS_TEST_CHANGE_DISTANCE(0, win->ui, false);
            default:
                RESET_SHOW_DIST_DATA(p, win->ui);
            }

            usrFrame.show();
            win->ui->canvas->syncWithUiFrame(&usrFrame);

            modelStore.calcPosType = CALC_POS_TYPE::Mp_Pos_In;
            static_cast<dataInputPos *>(dataInputHandler)->startMpReqTimer();
        } else {
            static_cast<dataInputPos *>(dataInputHandler)->stopMpReqTimer();
        }
    }
}
