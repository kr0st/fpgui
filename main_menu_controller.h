#ifndef MAIN_MENU_CONTROLLER_H
#define MAIN_MENU_CONTROLLER_H

#include <table_controller.h>
#include <main_menu_view.h>

class Main_Menu_Controller: public fpgui::ui::Table_Controller
{
    Q_OBJECT

    public:

        Main_Menu_Controller(fpgui::ui::Main_Menu_View& view);


    public slots:

        void on_history_browse();
        void on_datetime_changed(qint64 start_datetime, qint64 end_datetime);
};

#endif // MAIN_MENU_CONTROLLER_H
