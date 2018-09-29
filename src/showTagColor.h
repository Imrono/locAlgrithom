#ifndef SHOWTAGCOLOR_H
#define SHOWTAGCOLOR_H
#include <QMap>
#include <QColor>
#include "_myheads.h"

struct oneTagView {
    QColor color[2];
    SHOW_SHAPE type[2];
    int nColorStyle;
    bool isUsed;
};

struct tagsView {
    tagsView();
    QList<oneTagView> viewDatabase;
    int count;
};

class showTagColor
{
public:
    static showTagColor* getInstance();

private:
    showTagColor();
    static showTagColor* instance;

    QMap<int, oneTagView> tagViewData;
    tagsView              tagsViewDataBase;

    // static Max Likehood sigma
    static dType sigmaLM;
public:
    void recordTagId(int tagId);
    void eraseTagId(int tagId);
    oneTagView getTagId(int tagId);

    void resetColorCount() {tagsViewDataBase.count = 0;}
};

#endif // SHOWTAGCOLOR_H
