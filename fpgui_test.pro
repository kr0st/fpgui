QT += core
QT -= gui

CONFIG += c++14

TARGET = fpgui_test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += "$$_PRO_FILE_PWD_/dependencies/include/"

LIBS += -L"$$_PRO_FILE_PWD_/dependencies/lib/x64/"
LIBS += -L/usr/local/lib/
LIBS += -lmongocxx -lgtest -lluajit

SOURCES += \
    fpgui_test.cpp \
    globals.cpp \
    settings.cpp \
    mac_util.cpp \
    simplecrypt.cpp \
    utils.cpp \
    scripting.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
           QT_NO_DEBUG_OUTPUT \
           QT_NO_INFO_OUTPUT \
           QT_NO_WARNING_OUTPUT \
           QT_MESSAGELOGCONTEXT \
           _LINUX \
           RAPIDJSON_HAS_STDSTRING


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    globals.h \
    settings.h \
    mac_util.h \
    simplecrypt.h \
    utils.h \
    scripting.h

QMAKE_LFLAGS += -pagezero_size 10000 -image_base 100000000 -Wl,-stack_size,0x20000000
