#-------------------------------------------------
#
# Project created by QtCreator 2017-12-13T10:29:36
#
#-------------------------------------------------

QT       += core gui
#CONFIG   += console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
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
        main.cpp \
    showStore.cpp \
    uiCanvas.cpp \
    uiMainWindow.cpp \
    dataSensorIni.cpp \
    showTagRelated.cpp \
    dataDistanceLog.cpp \
    calcTagPos.cpp \
    calcTagNLOS.cpp \
    uiUsrInfoBtn.cpp \
    uiUsrFrame.cpp \
    calcLibmatrixOp.cpp \
    calcLibDistance.cpp \
    _myheads.cpp \
    calcTagTrack.cpp

HEADERS += \
    showStore.h \
    _myheads.h \
    uiMainWindow.h \
    uiCanvas.h \
    dataSensorIni.h \
    showTagRelated.h \
    dataDistanceLog.h \
    calcTagPos.h \
    calcTagNLOS.h \
    dataType.h \
    uiUsrInfoBtn.h \
    uiUsrFrame.h \
    calcLibDistance.h \
    calcLibMatrixOp.h \
    calcTagTrack.h

FORMS += \
    mainwindow.ui



#win32-msvc*:QMAKE_CXXFLAGS += /wd"4819"
win64-msvc*:QMAKE_CXXFLAGS += /wd"4819"
win32-msvc*:QMAKE_CXXFLAGS += /wd"4819"

RESOURCES += \
    resource.qrc
