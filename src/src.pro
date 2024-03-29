QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialterminal
CONFIG += c++11

CONFIG(debug, debug|release):CONFIGURATION=debug

CONFIG(release, debug|release){
    CONFIGURATION=release
    #from static build
    QMAKE_LFLAGS_RELEASE += -static -static-libgcc
}

build_pass:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,_debug)
}

DEFINES += QT_DEPRECATED_WARNINGS

OBJECTS_DIR         = ../build/$$CONFIGURATION
MOC_DIR             = ../build/$$CONFIGURATION
RCC_DIR             = ../build/$$CONFIGURATION
UI_DIR              = ../build/$$CONFIGURATION
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
    consoleWidget.cpp \
    windows/modbuspacketcreator.cpp

HEADERS += \
    global.h \
    mainwindow.h \
    consoleWidget.h \
    windows/modbuspacketcreator.h

FORMS += \
    mainwindow.ui \
    windows/modbuspacketcreator.ui

RESOURCES += resources.qrc

TRANSLATIONS = lang/ru_RU.ts

exists(./gitversion.pri):include(./gitversion.pri)
exists(./myLibs.pri):include(./myLibs.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
