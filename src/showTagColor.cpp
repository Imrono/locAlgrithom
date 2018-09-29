#include "showTagColor.h"

tagsView::tagsView(){
    viewDatabase << oneTagView{{QColor{Qt::green},  QColor{Qt::darkGreen}},  {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 0, false};
    viewDatabase << oneTagView{{QColor{Qt::red},    QColor{Qt::darkRed}},    {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 1, false};
    viewDatabase << oneTagView{{QColor{Qt::blue},   QColor{Qt::darkBlue}},   {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 2, false};
    viewDatabase << oneTagView{{QColor{Qt::cyan},   QColor{Qt::darkCyan}},   {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 3, false};
    viewDatabase << oneTagView{{QColor{Qt::magenta},QColor{Qt::darkMagenta}},{SHOW_SHAPE::radius, SHOW_SHAPE::square}, 4, false};
    viewDatabase << oneTagView{{QColor{Qt::yellow}, QColor{Qt::darkYellow}}, {SHOW_SHAPE::radius, SHOW_SHAPE::square}, 5, false};
    count = 0;
}

showTagColor* showTagColor::instance = new showTagColor();
showTagColor* showTagColor::getInstance() {
    return instance;
}

showTagColor::showTagColor() : tagsViewDataBase()
{}

void showTagColor::recordTagId(int tagId) {
    if (!tagViewData.contains(tagId)) {
        for (int i = 0; i < tagsViewDataBase.viewDatabase.count(); i++) {
            if (false == tagsViewDataBase.viewDatabase[i].isUsed) {
                tagsViewDataBase.viewDatabase[i].isUsed = true;
                tagViewData.insert(tagId, tagsViewDataBase.viewDatabase[i]);
                tagsViewDataBase.count++;
                qDebug() << "[@recordTagId] insert color" << tagId;
                break;
            }
        }
    }
}

void showTagColor::eraseTagId(int tagId) {
    if (tagViewData.contains(tagId)) {
        tagsViewDataBase.viewDatabase[tagViewData[tagId].nColorStyle].isUsed = false;
        tagViewData.remove(tagId);
        tagsViewDataBase.count--;
        qDebug() << "[@eraseTagId] remove color" << tagId;
    }
}

oneTagView showTagColor::getTagId(int tagId) {
    oneTagView ret;
    if (tagViewData.contains(tagId)) {
        ret = tagViewData[tagId];
    } else {
        ret.isUsed = false;
    }
    return ret;
}
