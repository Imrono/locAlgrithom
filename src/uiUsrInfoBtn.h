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
    USR_STATUS getUsrStatus() {
        return status;
    }
    int  getTagId()    { return tagId;}
    void setBtnEnableLM(bool enable) {
        isEnableLM = enable;
    }

    bool getShowable() { return isShowable;}
    void setShowable(bool in) { isShowable = in; syncShowable();}
    void setShowToolTip(bool isShow,
                        const int *distance = nullptr,
                        const dType * weight = nullptr,
                        const locationCoor *sensor = nullptr,
                        const int N = 0,
                        QPointF real = QPointF(0.f, 0.f), QPointF canvas = QPointF(0.f, 0.f));

    void setColorSample(const QColor &color) {colorSample = color;}
    QColor getColorSample() {return colorSample;}

    void setIsShowingDist(bool isShowing) {isShowingDist = isShowing;}

signals:
    void oneUsrBtnClicked(int tagId);
    void oneUsrShowML(int tagId);   // ML -> maximum likehood
    void oneUsrShowDistance(int tagId);

private:
    void initial();
    void syncShowable();

    int tagId;
    bool isEnableLM{false};
    bool isShowable{true};  // if enabled, autochanged during btn click
                            // also setShowable can change it
    USR_STATUS status;

    void paintEvent(QPaintEvent *event);
    void keyPressEvent (QKeyEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);

    QColor colorSample;

    QMenu *contextMenu{nullptr};
    QAction *showML_Action{nullptr};
    bool isShowingML{false};
    QAction *showDistInfo{nullptr};

    bool isShowToolTip{false};
    QPointF posReal;
    QPointF posCanvas;

    bool isShowingDist{false};
};

#endif // UIUSRINFOBTN_H
