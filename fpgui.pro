#-------------------------------------------------
#
# Project created by QtCreator 2017-08-24T15:06:45
#
#-------------------------------------------------

QT       += core gui
QT += network

CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fpgui
TEMPLATE = app

INCLUDEPATH += "$$_PRO_FILE_PWD_/dependencies/include/"
INCLUDEPATH += "$$_PRO_FILE_PWD_/headers/"
INCLUDEPATH += "/usr/local/include/"
INCLUDEPATH += "/usr/local/include/mongocxx/v_noabi/"
INCLUDEPATH += "/usr/local/include/bsoncxx/v_noabi/"

LIBS += -L"$$_PRO_FILE_PWD_/dependencies/lib/x64/"
LIBS += -L/usr/local/lib/
LIBS += -lmongocxx -lbsoncxx -lgtest -lluajit

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
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


SOURCES += \
    sources/main.cpp \
    sources/mainwindow.cpp \
    sources/settings.cpp \
    sources/var_injector.cpp \
    sources/scripting.cpp \
    sources/utils.cpp \
    sources/globals.cpp \
    sources/simplecrypt.cpp \
    sources/table_view.cpp \
    sources/table_controller.cpp \
    sources/mongo_data_source.cpp \
    sources/historybrowserwindow.cpp \
    sources/history_browser_view.cpp \
    sources/history_browser_controller.cpp \
    sources/key_press_emitter.cpp \
    sources/message_details_dialog.cpp \
    sources/preferences.cpp \
    sources/tabs_configuration.cpp \
    sources/add_new_tab.cpp \
    sources/app_settings_advanced.cpp \
    sources/db_settings_advanced.cpp \
    sources/hsv_rgb_converter.cpp \
    sources/colorizer.cpp \
    sources/highlighting_settings.cpp

HEADERS += \
    headers/mainwindow.h \
    headers/scripting.h \
    headers/settings.h \
    headers/var_injector.h \
    headers/utils.h \
    headers/globals.h \
    headers/simplecrypt.h \
    headers/table_view.h \
    headers/table_controller.h \
    headers/data_source.h \
    headers/mongo_data_source.h \
    headers/fpgui_exceptions.h \
    headers/historybrowserwindow.h \
    headers/windowwithmessagebox.h \
    headers/history_browser_view.h \
    headers/history_browser_controller.h \
    headers/key_press_emitter.h \
    headers/message_details_dialog.h \
    headers/preferences.h \
    headers/tabs_configuration.h \
    headers/add_new_tab.h \
    headers/app_settings_advanced.h \
    headers/db_settings_advanced.h \
    headers/hsv_rgb_converter.h \
    headers/colorizer.h \
    headers/highlighting_settings.h

FORMS += \
    forms/mainwindow.ui \
    forms/historybrowserwindow.ui \
    forms/message_details_dialog.ui \
    forms/preferences.ui \
    forms/tabs_configuration.ui \
    forms/add_new_tab.ui \
    forms/app_settings_advanced.ui \
    forms/db_settings_advanced.ui \
    forms/highlighting_settings.ui

QMAKE_LFLAGS += -pagezero_size 10000 -image_base 100000000 -Wl,-stack_size,0x20000000
