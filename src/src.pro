QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialTerminal
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

OBJECTS_DIR         = ../build
MOC_DIR             = ../build
RCC_DIR             = ../build
DESTDIR             = ../bin

win32|win64{
    RC_FILE=  index.rc
    OTHER_FILES+= index.rc
    DISTFILES += index.rc
}

SOURCES += \
    global.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    global.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += resources.qrc \
    resources.qrc

TRANSLATIONS = lang/ru_RU.ts


exists(./gitversion.pri):include(./gitversion.pri)
exists(./myLibs.pri):include(./myLibs.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    gitversion.pri
