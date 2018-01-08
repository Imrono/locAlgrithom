#ifndef UIUSRINFOBTN_H
#define UIUSRINFOBTN_H
#include "_myheads.h"
#include <QToolButton>

class uiUsrInfoBtn : public QToolButton
{
    Q_OBJECT
public:
    uiUsrInfoBtn(int tagId, QWidget *parent = 0);
    uiUsrInfoBtn(int tagId, bool isShowable, QWidget *parent = 0);

    void setUsrStatus(USR_STATUS status);
    int  getTagId()    { return tagId;}
    bool getShowable() { return isShowable;}
    void setShowable(bool in) { isShowable = in; syncShowable();}
signals:
    void oneUsrBtnClicked(int tagId);

private:
    void initial();
    void syncShowable();

    int tagId;
    bool isShowable{true};
};

#endif // UIUSRINFOBTN_H
