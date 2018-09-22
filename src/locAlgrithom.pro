#-------------------------------------------------
#
# Project created by QtCreator 2017-12-13T10:29:36
#
#-------------------------------------------------

QT       += core gui
#CONFIG   += console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

CONFIG += qwt

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
        main.cpp \
    uiCanvas.cpp \
    uiMainWindow.cpp \
    dataSensorIni.cpp \
    dataDistanceLog.cpp \
    calcTagPos.cpp \
    calcTagNLOS.cpp \
    uiUsrInfoBtn.cpp \
    uiUsrFrame.cpp \
    _myheads.cpp \
    calcTagTrack.cpp \
    uiMainwindowTedious.cpp \
    calcLibMatrixOp.cpp \
    calcLibMath.cpp \
    calcLibMatrixOpInverse.cpp \
    armVersion/calcLibMath_ARM.c \
    armVersion/calcTagTrack_ARM.c \
    armVersion/calcTagPos_ARM.c \
    calcTagPosLMedS.cpp \
    calcTagPosBilateration.cpp \
    calcTagPosWeightedTaylor.cpp \
    calcLibGeometry.cpp \
    calcTagPosKalmanCoupled.cpp \
    showTagDelegate.cpp \
    showTagModel.cpp \
    _calcParam.cpp \
    zFakeData.cpp \
    uiShowItem.cpp \
    uiUsrTooltip.cpp

HEADERS += \
    _myheads.h \
    uiMainWindow.h \
    uiCanvas.h \
    dataSensorIni.h \
    dataDistanceLog.h \
    calcTagPos.h \
    calcTagNLOS.h \
    dataType.h \
    uiUsrInfoBtn.h \
    uiUsrFrame.h \
    calcLibMatrixOp.h \
    calcTagTrack.h \
    calcLibMath.h \
    armVersion/calcTagLoc_ARM.h \
    calcLibGeometry.h \
    showTagDelegate.h \
    showTagModel.h \
    _calcParam.h \
    zFakeData.h \
    uiShowItem.h \
    uiUsrTooltip.h

FORMS += \
    mainwindow.ui \
    usrtooltip.ui

QMAKE_CXXFLAGS += /wd"4819"
QMAKE_CFLAGS += /wd"4819"

RESOURCES += \
    resource.qrc

DISTFILES +=
RC_ICONS = resource/icon/locAlg.ico

INCLUDEPATH += "C:/Qt/Qt5.9.1/5.9.1/msvc2015_64/include/qwt-6.1.3/"
DEFINES += QWT_DLL
CONFIG(debug, debug|release) {
    unix|win32: LIBS += -L"C:\Qt\Qt5.9.1\5.9.1\msvc2015_64\lib" -lqwtd
} else {
    unix|win32: LIBS += -L"C:\Qt\Qt5.9.1\5.9.1\msvc2015_64\lib" -lqwt
}
