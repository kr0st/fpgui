#include "table_view.h"

#include <vector>

namespace fpgui {
namespace ui {

void Table_View::setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget &widget)
{
    widget.setColumnCount(config.size());
    widget.setRowCount(1);
    int col = 0;
    for (auto& tab : config)
    {
        widget.setHorizontalHeaderItem(col++, new QTableWidgetItem(tab.name.c_str()));
    }
}

void Table_View::close_view()
{
    emit closing();
}

}}
