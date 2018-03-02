# MEMS-oriented-image-testing-technology

TEMPLATE = app
TARGET = mems
DESTDIR = ./

VERSION = 1.0.1

CONFIG += c++14
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # disables all the APIs deprecated before Qt 6.0.0
QT += core gui widgets

translationDir = translations
DEFINES += $$shell_quote(TRANSLATIONS_DIR=\"$$translationDir\")

# Copies the given files to the destination directory
defineTest(copyToDestdir) {
    FILES = $$1
    for(FILE, FILES) {
        DDIR = $$shadowed($$FILE)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}

SOURCES += main.cpp \
    mainpanel.cpp \
    imagefilter.cpp \
    thresholding.cpp \
    edgedetect.cpp \
    circlefit.cpp \
    processor.cpp \
    configuration.cpp


HEADERS += \
    mainpanel.h \
    binarize.hpp \
    imagefilter.h \
    thresholding.h \
    edgedetect.h \
    circlefit.h \
    processor.h \
    configuration.h \
    algorithms.h

FORMS += \
    mainpanel.ui

DISTFILES += \
    README.md

RESOURCES += \
    images.qrc

LOCALES += \
    zh_CN

QMAKE_PRE_LINK += $$[QT_INSTALL_BINS]/lupdate $$_PRO_FILE_ $$escape_expand(\\n\\t)

QMAKE_PRE_LINK += mkdir $$shell_path($$shadowed($${translationDir})) $$escape_expand(\\n\\t)

copyToDestdir($$absolute_path(config.ini))

for(LOCALE, LOCALES) {
    TRANSLATIONS += mems_$${LOCALE}.ts
    QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/lrelease $$absolute_path(mems_$${LOCALE}.ts) \
         -qm $$shadowed($${translationDir}/mems_$${LOCALE}.qm) $$escape_expand(\\n\\t)
}
