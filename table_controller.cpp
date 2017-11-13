#include "table_controller.h"
#include <utils.h>

namespace fpgui {
namespace ui {

void Table_Controller::on_view_closing()
{
    generic_utils::ui::message_box("closing app!");
}

}
}
