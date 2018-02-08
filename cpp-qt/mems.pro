# MEMS-oriented-image-testing-technology

TEMPLATE = app
TARGET = mems

#VERSION = 1.0.0

CONFIG += c++14
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # disables all the APIs deprecated before Qt 6.0.0
QT += core gui widgets

SOURCES += main.cpp \
    mainpanel.cpp \
    imagefilter.cpp \
    thresholding.cpp \
    edgedetect.cpp \
    circlefit.cpp \
    processor.cpp


HEADERS += \
    mainpanel.h \
    binarize.hpp \
    imagefilter.h \
    thresholding.h \
    edgedetect.h \
    circlefit.h \
    algorithm.h \
    processor.h

FORMS += \
    mainpanel.ui

DISTFILES += \
    README.md

RESOURCES += \
    images.qrc
