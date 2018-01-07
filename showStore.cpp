#include "showStore.h"

showStore::showStore()
{
}

void showStore::addNewTagInfo(int tagId) {
    if (!tags.contains(tagId)) {
        storeTagInfo *tag = new storeTagInfo;
        tag->tagId = tagId;
        tags.insert(tagId, tag);
    }
}

storeTagInfo *showStore::getTagInfo(int tagId) {
    if (tags.contains(tagId)) {
        return tags[tagId];
    } else {
        return nullptr;
    }
}
void addNewMethodInfo(int tagId, const QString &method) {

}

void showStore::addRawPoints(int tagId, QVector<locationCoor> points) {
    if (tags.contains(tagId))
        tags[tagId]->RawPoints.append(points);
}

void showStore::clearRawPoints(int tagId) {
    if (tags.contains(tagId))
        tags[tagId]->RawPoints.clear();
}

void showStore::addAnsPoint(int tagId, const QString &method, locationCoor p) {
    if (tags.contains(tagId))
        if(tags[tagId]->methodInfo.contains(method))
            tags[tagId]->methodInfo[method].Ans.append(p);
}

void showStore::clearAnsPoints(int tagId, const QString &method) {
    if (tags.contains(tagId))
        if(tags[tagId]->methodInfo.contains(method))
            tags[tagId]->methodInfo[method].Ans.clear();
}
