#include "table_controller.h"
#include <utils.h>

namespace fpgui {
namespace ui {

void Table_Controller::on_view_closing()
{
    QSettings settings;
    fpgui::settings::write_tab_config(view_.get_view_configuration(), settings);
}

}
}
