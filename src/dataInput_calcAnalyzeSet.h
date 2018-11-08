#ifndef DATAINPUT_CALCANALYZESET_H
#define DATAINPUT_CALCANALYZESET_H
#include "showTagModel.h"
#include "uiUsrFrame.h"
#include "dataInputBase.h"
#include "dataInputLog.h"
#include "dataInputTest.h"
#include "dataInputCan.h"
#include "dataInputPos.h"

class uiMainWindow;
class dataInput_calcAnalyzeSet {
public:
    dataInput_calcAnalyzeSet(int type, uiMainWindow *win);

    dataInputBase *dataInputHandler{nullptr};
    int dataInputType;
    showTagModel modelStore;
    uiUsrFrame usrFrame;
    bool isActive{false};
    QString strName;
    int counting{0};

    void handleModelChange(bool isSelected, uiMainWindow *win);
private:
    void handleModelChangeLog(bool isSelected, uiMainWindow *win);
    void handleModelChangeTest(bool isSelected, uiMainWindow *win);
    void handleModelChangeCan(bool isSelected, uiMainWindow *win);
    void handleModelChangePos(bool isSelected, uiMainWindow *win);
};

#endif // DATAINPUT_CALCANALYZESET_H
