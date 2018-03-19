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
    void setUsrStatus(int tagId, USR_STATUS status);

    bool isShowable(int tagId);
    QList<int> getShowableTags();

    void setBtnColorSample(int tagId, const QColor &color);

    void setBtnToolTip(int tagId, bool isShowPos,
                       QPointF real = QPointF(), QPointF canvas = QPointF());

    int getShowDistTagId() {return showDistTagId;}

signals:
    void oneUsrBtnClicked_siganl(int tagId, bool isShowable);
    void oneUsrShowML_siganl(int tagId, bool isShowable);

private slots:
    void oneUsrBtnClicked_slot(int tagId);
    void oneUsrShowML_slot(int tagId);

private:
    QList<uiUsrInfoBtn *> usrBtns;
    int nShowableBtns{0};

    const static int MAX_SHOWABLE_NUM;
    int tagShowLM{-1};

    int showDistTagId{-1};
};

#endif // UIUSRFRAME_H
