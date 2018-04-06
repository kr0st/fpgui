#include "main_menu_view.h"

namespace fpgui { namespace ui {

Main_Menu_View::Main_Menu_View(settings::App_Configuration& app_config):
Table_View(app_config)
{
    app_config_.view_sorting = true;
    app_config_.view_autoscroll = false;
}

void Main_Menu_View::on_history_browse()
{
    emit history_browse();
}

void Main_Menu_View::on_datetime_changed(qint64 start_datetime, qint64 end_datetime)
{
    emit datetime_changed(start_datetime, end_datetime);
}

}}
