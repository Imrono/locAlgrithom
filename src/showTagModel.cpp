#include "showTagModel.h"

void storeTagInfo::addOrResetMethodInfo(const QString &methodType, const QString &method) {
    if (!methodInfo.contains(methodType)) {
        methodInfo.insert(methodType, storeMethodInfo(method, this));
    } else {
        reset(methodType, method);
    }
}
void storeTagInfo::reset(const QString &methodType, const QString &method) {
    if (methodInfo.contains(methodType)) {
        methodInfo[methodType].clear();
        methodInfo[methodType].methodName = method;
    }
}
void storeTagInfo::reset(const QString &methodType) {
    if (methodInfo.contains(methodType)) {
        methodInfo[methodType].Ans.clear();
        methodInfo[methodType].AnsLines.clear();
    }
}

showTagModel::showTagModel()
{
}

void showTagModel::addNewTagInfo(int tagId) {
    if (!tags.contains(tagId)) {
        storeTagInfo *tag = new storeTagInfo;
        tag->tagId = tagId;
        tags.insert(tagId, tag);
    }
}

storeTagInfo *showTagModel::getTagInfo(int tagId) {
    if (tags.contains(tagId)) {
        return tags[tagId];
    } else {
        return nullptr;
    }
}

void showTagModel::addRawPoints(int tagId, QVector<locationCoor> points) {
    if (tags.contains(tagId))
        tags[tagId]->RawPoints.append(points);
}

void showTagModel::clearRawPoints(int tagId) {
    if (tags.contains(tagId))
        tags[tagId]->RawPoints.clear();
}

void showTagModel::addAnsPoint(int tagId, const QString &method, locationCoor p) {
    if (tags.contains(tagId))
        if(tags[tagId]->methodInfo.contains(method))
            tags[tagId]->methodInfo[method].Ans.append(p);
}

void showTagModel::clearAnsPoints(int tagId, const QString &method) {
    if (tags.contains(tagId))
        if(tags[tagId]->methodInfo.contains(method))
            tags[tagId]->methodInfo[method].Ans.clear();
}
