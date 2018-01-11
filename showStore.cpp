#include "showStore.h"

void storeTagInfo::addOrResetMethodInfo(const QString &methodType, const QString &method) {
    if (!methodInfo.contains(methodType)) {
        methodInfo.insert(methodType, storeMethodInfo(method, this));
    } else {
        reset(methodType, method);
    }
}
void storeTagInfo::reset(const QString &methodType, const QString &method) {
    if (methodInfo.contains(methodType)) {
        methodInfo[methodType].Ans.clear();
        methodInfo[methodType].AnsLines.clear();
        methodInfo[methodType].methodName = method;
    }
}
void storeTagInfo::reset(const QString &methodType) {
    if (methodInfo.contains(methodType)) {
        methodInfo[methodType].Ans.clear();
        methodInfo[methodType].AnsLines.clear();
    }
}

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
