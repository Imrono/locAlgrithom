#-------------------------------------------------
#
# Project created by QtCreator 2018-01-24T14:22:33
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_locAlgrithom
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    calcLibMatrixOp_TEST.cpp \
    ../src/_myheads.cpp \
    ../src/calcLibMatrixOp.cpp \
    ../src/calcLibMath.cpp \
    ../src/calclibMatrixOpInverse.cpp \
    calcLibMath_ARM_TEST.cpp \
    ../src/armVersion/calcLibMath_ARM.c \
    ../src/armVersion/calcTagPos_ARM.c \
    testMain.cpp \
    calcLibMath_TEST.cpp \
    ../src/calcLibGeometry.cpp \
    calcLibGeometry_TEST.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    calcLibMatrixOp_TEST.h \
    ../src/_myheads.h \
    ../src/calcLibMatrixOp.h \
    ../src/calcLibMath.h \
    calcLibMath_ARM_TEST.h \
    ../src/armVersion/calcTagPos_ARM.h \
    calcLibMath_TEST.h \
    ../src/calcLibGeometry.h \
    ../src/calcLibGeometry.h \
    calcLibGeometry_TEST.h

QMAKE_CXXFLAGS += /wd"4819"
QMAKE_CFLAGS += /wd"4819"
