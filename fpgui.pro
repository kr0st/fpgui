#-------------------------------------------------
#
# Project created by QtCreator 2017-08-24T15:06:45
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fpgui
TEMPLATE = app

INCLUDEPATH += "$$_PRO_FILE_PWD_/dependencies/include/"
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
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    var_injector.cpp \
    scripting.cpp \
    utils.cpp \
    globals.cpp \
    simplecrypt.cpp \
    mac_util.cpp \
    table_view.cpp \
    table_controller.cpp \
    mongo_data_source.cpp \
    historybrowserwindow.cpp \
    history_browser_view.cpp \
    history_browser_controller.cpp \
    key_press_emitter.cpp \
    message_details_dialog.cpp \
    preferences.cpp \
    tabs_configuration.cpp \
    add_new_tab.cpp \
    app_settings_advanced.cpp

HEADERS += \
    mainwindow.h \
    scripting.h \
    settings.h \
    var_injector.h \
    utils.h \
    globals.h \
    simplecrypt.h \
    mac_util.h \
    table_view.h \
    table_controller.h \
    data_source.h \
    mongo_data_source.h \
    fpgui_exceptions.h \
    historybrowserwindow.h \
    windowwithmessagebox.h \
    history_browser_view.h \
    history_browser_controller.h \
    key_press_emitter.h \
    message_details_dialog.h \
    preferences.h \
    tabs_configuration.h \
    add_new_tab.h \
    app_settings_advanced.h

FORMS += \
        mainwindow.ui \
    historybrowserwindow.ui \
    message_details_dialog.ui \
    preferences.ui \
    tabs_configuration.ui \
    add_new_tab.ui \
    app_settings_advanced.ui

QMAKE_LFLAGS += -pagezero_size 10000 -image_base 100000000 -Wl,-stack_size,0x20000000
