#include "main_menu_controller.h"

Main_Menu_Controller::Main_Menu_Controller(fpgui::ui::Main_Menu_View &view):
Table_Controller(view)
{
    connect(&view_, SIGNAL(history_browse()), this, SLOT(on_history_browse()));
}

void Main_Menu_Controller::on_history_browse()
{
    view_.show_hide();
}
