#ifndef MAIN_MENU_VIEW_H
#define MAIN_MENU_VIEW_H

#include <QObject>
#include <table_controller.h>
#include <settings.h>

namespace fpgui { namespace ui {

class Main_Menu_View : public Table_View
{
    Q_OBJECT


    public:

        Main_Menu_View(settings::App_Configuration& app_config);


    signals:

        void history_browse();


    public slots:

        void on_history_browse();
};

}}

#endif // MAIN_MENU_VIEW_H
