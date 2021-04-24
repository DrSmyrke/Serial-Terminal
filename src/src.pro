QT       += core gui serialport

##################################################
#uncomment from static build
#QMAKE_LFLAGS_RELEASE += -static -static-libgcc
##################################################

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialterminal
CONFIG += c++11

CONFIG(debug, debug|release):CONFIGURATION=debug
CONFIG(release, debug|release):CONFIGURATION=release

build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,_debug)
    else: TARGET = $$join(TARGET,,,d)
}

DEFINES += QT_DEPRECATED_WARNINGS

OBJECTS_DIR         = ../build
MOC_DIR             = ../build
RCC_DIR             = ../build
UI_DIR              = ../build
DESTDIR             = ../bin

win32|win64{
    RC_FILE=  index.rc
    OTHER_FILES+= index.rc
    DISTFILES += index.rc
}

SOURCES += \
    global.cpp \
    main.cpp \
    mainwindow.cpp \
    consoleWidget.cpp

HEADERS += \
    global.h \
    mainwindow.h \
    consoleWidget.h

FORMS += \
    mainwindow.ui

RESOURCES += resources.qrc

TRANSLATIONS = lang/ru_RU.ts

exists(./gitversion.pri):include(./gitversion.pri)
exists(./myLibs.pri):include(./myLibs.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
