#include "main_menu_controller.h"

Main_Menu_Controller::Main_Menu_Controller(fpgui::ui::Table_View& view):
Table_Controller(view)
{
}

Main_Menu_Controller::on_history_browse()
{
    view_.show_hide();
}
