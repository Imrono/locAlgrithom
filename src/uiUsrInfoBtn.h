#ifndef UIUSRINFOBTN_H
#define UIUSRINFOBTN_H
#include "_myheads.h"
#include <QToolButton>
#include <QMenu>
#include <QAction>

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

    void setColorA(const QColor &color) {colorA = color; isColorA_ready = true;}
    void clrColorA() {isColorA_ready = false;}
signals:
    void oneUsrBtnClicked(int tagId);
    void oneUsrShowML(int tagId);

private:
    void initial();
    void syncShowable();

    int tagId;
    bool isShowable{true};

    void paintEvent(QPaintEvent *event);
    void keyPressEvent (QKeyEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);

    QColor colorA;
    bool isColorA_ready{false};

    QMenu *contextMenu{nullptr};
    QAction *showML_Action{nullptr};
    bool isShowingML{false};
};

#endif // UIUSRINFOBTN_H
