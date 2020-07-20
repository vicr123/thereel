QT       += core gui thelib multimediawidgets
SHARE_APP_NAME = thereel

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = thereel

# Include the-libs build tools
include(/usr/share/the-libs/pri/buildmaster.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$PWD/translations) $$shell_quote($$OUT_PWD) && \
    $$QMAKE_COPY $$quote($$PWD/defaults.conf) $$shell_quote($$OUT_PWD)

SOURCES += \
    controlstrip.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    controlstrip.h \
    mainwindow.h

FORMS += \
    controlstrip.ui \
    mainwindow.ui

unix {
    target.path = $$[QT_INSTALL_BINS]

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theReel/

    desktop.path = /usr/share/applications
    desktop.files = com.vicr123.thereel.desktop

    icon.path = /usr/share/icons/hicolor/scalable/apps/
    icon.files = icons/thereel.svg

    INSTALLS += target defaults icon desktop
}

RESOURCES += \
    resources.qrc

DISTFILES += \
    com.vicr123.thereel.desktop \
    defaults.conf
