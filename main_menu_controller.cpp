#include "main_menu_controller.h"

Main_Menu_Controller::Main_Menu_Controller(fpgui::ui::Main_Menu_View &view):
Table_Controller(view)
{
    connect(&view_, SIGNAL(history_browse()), this, SLOT(on_history_browse()));
    connect(&view_, SIGNAL(datetime_changed(qint64, qint64)), this, SLOT(on_datetime_changed(qint64, qint64)));
}

void Main_Menu_Controller::on_history_browse()
{
    view_.show();
}

void Main_Menu_Controller::on_datetime_changed(qint64 start_datetime, qint64 end_datetime)
{
    stop_refreshing_view();
}
