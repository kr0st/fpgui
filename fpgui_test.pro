QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = fpgui_test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += "$$_PRO_FILE_PWD_/dependencies/include/"
INCLUDEPATH += "/usr/local/include/"
INCLUDEPATH += "/usr/local/include/mongocxx/v_noabi/"
INCLUDEPATH += "/usr/local/include/bsoncxx/v_noabi/"

LIBS += -L"$$_PRO_FILE_PWD_/dependencies/lib/x64/"
LIBS += -L/usr/local/lib/
LIBS += -lmongocxx -lbsoncxx -lgtest -lluajit

SOURCES += \
    sources/fpgui_test.cpp \
    sources/globals.cpp \
    sources/settings.cpp \
    sources/mac_util.cpp \
    sources/simplecrypt.cpp \
    sources/utils.cpp \
    sources/scripting.cpp \
    sources/var_injector.cpp \
    sources/table_view.cpp \
    sources/table_controller.cpp \
    sources/mainwindow.cpp \
    sources/data_source.cpp \
    sources/business_logic_test_data.cpp \
    sources/mongo_data_source.cpp

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
           RAPIDJSON_HAS_STDSTRING \
           _UNIT_TEST


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    headers/globals.h \
    headers/settings.h \
    headers/mac_util.h \
    headers/simplecrypt.h \
    headers/utils.h \
    headers/scripting.h \
    headers/var_injector.h \
    headers/table_controller.h \
    headers/mainwindow.h \
    headers/table_view.h \
    headers/data_source.h \
    headers/business_logic_test_data.h \
    headers/mongo_data_source.h

FORMS += forms/mainwindow.ui

QMAKE_LFLAGS += -pagezero_size 10000 -image_base 100000000 -Wl,-stack_size,0x20000000

