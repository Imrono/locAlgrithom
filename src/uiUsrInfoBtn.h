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
    void setShowPos(bool isShow, QPointF real = QPointF(0.f, 0.f), QPointF canvas = QPointF(0.f, 0.f));

    void setColorSample(const QColor &color) {colorSample = color;}
signals:
    void oneUsrBtnClicked(int tagId);
    void oneUsrShowML(int tagId);   // ML -> maximum likehood

private:
    void initial();
    void syncShowable();

    int tagId;
    bool isShowable{true};  // if enabled, autochanged during btn click
                            // also setShowable can change it

    void paintEvent(QPaintEvent *event);
    void keyPressEvent (QKeyEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);

    QColor colorSample;

    QMenu *contextMenu{nullptr};
    QAction *showML_Action{nullptr};
    bool isShowingML{false};

    bool isShowPos{false};
    QPointF posReal;
    QPointF posCanvas;
};

#endif // UIUSRINFOBTN_H
