# MEMS-oriented-image-testing-technology

TEMPLATE = app
TARGET = mems
DESTDIR = ./

VERSION = 1.0.1

CONFIG += c++14
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # disables all the APIs deprecated before Qt 6.0.0
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += NO_TIMING_OUTPUT
}
QT += core gui widgets

translationDir = translations
settingFile = config.ini
DEFINES += $$shell_quote(TRANSLATIONS_DIR=\"$$translationDir\")
DEFINES += $$shell_quote(SettingFile=\"$$settingFile\")

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
    algorithms.h \
    global.h

FORMS += \
    mainpanel.ui

DISTFILES += \
    README.md

RESOURCES += \
    images.qrc

LOCALES += \
    zh_CN

FILES_TO_COPY = \
    $$absolute_path($$settingFile)

QMAKE_PRE_LINK += $$[QT_INSTALL_BINS]/lupdate $$_PRO_FILE_ $$escape_expand(\\n\\t)

!exists($$shadowed($${translationDir})): \
    QMAKE_PRE_LINK += mkdir $$shell_path($$shadowed($${translationDir})) $$escape_expand(\\n\\t)

# Copies the given files to the destination directory
for(FILE, FILES_TO_COPY) {
    DDIR = $$shadowed($$FILE)
    QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$shell_path($$FILE)) \
         $$quote($$shell_path($$DDIR)) $$escape_expand(\\n\\t)
    QMAKE_CLEAN += $$shell_path($$DDIR)
}

for(LOCALE, LOCALES) {
    TRANSLATIONS += mems_$${LOCALE}.ts
    QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/lrelease $$absolute_path(mems_$${LOCALE}.ts) \
         -qm $$shadowed($${translationDir}/mems_$${LOCALE}.qm) $$escape_expand(\\n\\t)
    QMAKE_CLEAN += $$shadowed($${translationDir}/mems_$${LOCALE}.qm)
}
