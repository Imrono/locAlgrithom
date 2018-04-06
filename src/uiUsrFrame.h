#ifndef UIUSRFRAME_H
#define UIUSRFRAME_H
#include "_myheads.h"
#include <QFrame>
#include "uiUsrInfoBtn.h"

#define MAX_SHOWABLE(x) (MAX_SHOWABLE_NUM > nShowableBtns)

class uiUsrFrame : public QFrame
{
    Q_OBJECT
public:
    uiUsrFrame(QWidget *parent = 0);

    void addOneUsr(int tagId, USR_STATUS status = USR_STATUS::HAS_NONE_DATA);
    void removeOneUsr(int tagId);
    void removeAll();
    void setEnabledAll(bool enable);
    void setUsrStatus(int tagId, USR_STATUS status);
    USR_STATUS getUsrStatus(int tagId);

    bool isShowable(int tagId);
    QList<int> getShowableTags();

    void setBtnColorSample(int tagId, const QColor &color);
    QColor getBtnColorSample(int tagId);
    void setBtnEnableLM(int tagId, bool enable);

    void setBtnToolTip(int tagId, bool isShowPos,
                       const int *distance = nullptr,
                       const dType * weight = nullptr,
                       const locationCoor *sensor = nullptr,
                       const int N = 0,
                       QPointF real = QPointF(), QPointF canvas = QPointF());

    void setShowDistTagId(int tagId);
    int getShowDistTagId() {return showDistTagId;}

    int getTagShowLM() {return tagShowLM;}

signals:
    void oneUsrBtnClicked_siganl(int tagId, bool isShowable);
    void oneUsrShowML_siganl(int tagId, bool isShowable);
    void oneUsrShowDistance_siganl(int tagId);

private slots:
    void oneUsrBtnClicked_slot(int tagId);
    void oneUsrShowML_slot(int tagId);

private:
    QList<uiUsrInfoBtn *> usrBtns;
    int nShowableBtns{0};

    const static int MAX_SHOWABLE_NUM;
    int tagShowLM{UN_INIT_LM_TAGID};

    int showDistTagId{UN_INIT_SHOW_TAGID};
};

#endif // UIUSRFRAME_H
