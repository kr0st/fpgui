#include "main_menu_view.h"

namespace fpgui { namespace ui {

Main_Menu_View::Main_Menu_View(settings::App_Configuration& app_config):
Table_View(app_config)
{
}

void Main_Menu_View::on_history_browse()
{
    emit history_browse();
}

}}
