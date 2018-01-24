#-------------------------------------------------
#
# Project created by QtCreator 2017-12-13T10:29:36
#
#-------------------------------------------------

QT       += core gui
#CONFIG   += console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = locAlgrithom
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src/main.cpp \
    src/showStore.cpp \
    src/uiCanvas.cpp \
    src/uiMainWindow.cpp \
    src/dataSensorIni.cpp \
    src/showTagRelated.cpp \
    src/dataDistanceLog.cpp \
    src/calcTagPos.cpp \
    src/calcTagNLOS.cpp \
    src/uiUsrInfoBtn.cpp \
    src/uiUsrFrame.cpp \
    src/calcLibDistance.cpp \
    src/_myheads.cpp \
    src/calcTagTrack.cpp \
    src/uiMainwindowTedious.cpp \
    src/calcLibMatrixOp.cpp \
    src/calcTagPosKalmanTaylor.cpp \
    src/calcLibMath.cpp \
    src/calcTagPosWeightedTaylor.cpp \
    src/armVersion/calcTagPosWeightTaylor_ARM.c \
    src/calcLibMatrixOpInverse.cpp

HEADERS += \
    src/showStore.h \
    src/_myheads.h \
    src/uiMainWindow.h \
    src/uiCanvas.h \
    src/dataSensorIni.h \
    src/showTagRelated.h \
    src/dataDistanceLog.h \
    src/calcTagPos.h \
    src/calcTagNLOS.h \
    src/dataType.h \
    src/uiUsrInfoBtn.h \
    src/uiUsrFrame.h \
    src/calcLibDistance.h \
    src/calcLibMatrixOp.h \
    src/calcTagTrack.h \
    src/calcLibMath.h \
    src/armVersion/calcTagPosWeightTaylor_ARM.h

FORMS += \
    src/mainwindow.ui



#win32-msvc*:QMAKE_CXXFLAGS += /wd"4819"
win64-msvc*:QMAKE_CXXFLAGS += /wd"4819"
win32-msvc*:QMAKE_CXXFLAGS += /wd"4819"

RESOURCES += \
    src/resource.qrc

DISTFILES +=
RC_ICONS = resource/icon/locAlg.ico
