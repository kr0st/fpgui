#include "table_view.h"

#include <QHeaderView>

#include <vector>


namespace fpgui {
namespace ui {

static void balance_size_percentages(std::vector<settings::Tab_Configuration>& config, double total = 0)
{
    double min_tab_size = 100 / config.size();

    if (min_tab_size > 5)
        min_tab_size = 5;

    if (total < 0)
        total = 0;

    if (total < 0.1)
    {
        for (auto& tab : config)
        {
            if (tab.size < min_tab_size)
                tab.size = min_tab_size;
            total += tab.size;
        }

        if ((total < 100.5) && (total > 99))
            return;

        balance_size_percentages(config, total);
        return;
    }

    while (total > 100.5)
    {
        double temp = 0;

        for (auto& tab : config)
        {
            tab.size -= 0.1;

            if (tab.size < min_tab_size)
                tab.size = min_tab_size;

            temp += tab.size;
        }

        total = temp;
    }

    while (total < 100.5)
    {
        double temp = 0;

        for (auto& tab : config)
        {
            tab.size += 0.1;

            if (tab.size < min_tab_size)
                tab.size = min_tab_size;

            temp += tab.size;
        }

        total = temp;
    }
}

static void percentage_to_absolute_width(std::vector<settings::Tab_Configuration> &config, double widget_width)
{
    if (widget_width < 1)
        return;

    for (auto& tab : config)
        tab.size *= (widget_width / 100);
}

void Table_View::setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget &widget)
{
    double widget_width(widget.geometry().width());
    std::vector<settings::Tab_Configuration> config_copy(config);

    balance_size_percentages(config_copy);
    percentage_to_absolute_width(config_copy, widget_width);

    widget.setColumnCount(config_copy.size());
    widget.setRowCount(1);

    int col = 0;
    for (auto& tab : config_copy)
    {
        widget.setHorizontalHeaderItem(col++, new QTableWidgetItem(tab.name.c_str()));
        widget.setColumnWidth(col - 1, tab.size);
    }
}

void Table_View::close_view()
{
    emit closing();
}

std::vector<settings::Tab_Configuration> get_view_configuration()
{
}

}}
