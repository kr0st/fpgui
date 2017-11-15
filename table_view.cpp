#include "table_view.h"

#include <QHeaderView>

#include <vector>


namespace fpgui {
namespace ui {

static void remove_invisible_tabs(std::vector<settings::Tab_Configuration>& config)
{
    std::vector<settings::Tab_Configuration> tabs;

    for (auto& tab : config)
        if (tab.show)
            tabs.push_back(tab);

    config = tabs;
}

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

static void absolute_width_to_percentage(std::vector<settings::Tab_Configuration> &config, QTableWidget &widget)
{
    int cols = config.size();
    double widget_width(widget.geometry().width());

    for (int i = 0; i < cols; ++i)
    {
        double width = widget.columnWidth(i);
        config[i].size = width / widget_width * 100;
    }
}

void Table_View::setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget &widget, bool resize_only)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    double widget_width(widget.geometry().width());
    std::vector<settings::Tab_Configuration> config_copy(config);

    if (!resize_only)
    {
        remove_invisible_tabs(config_copy);
        balance_size_percentages(config_copy);

        config_ = config_copy;
        widget_ = &widget;

        widget.setColumnCount(config_copy.size());
        widget.setRowCount(1);
    }
    else
    {
        absolute_width_to_percentage(config_copy, widget);
        balance_size_percentages(config_copy);
        config_ = config_copy;
    }

    percentage_to_absolute_width(config_copy, widget_width);

    int col = 0;
    for (auto& tab : config_copy)
    {
        if (!resize_only)
            widget.setHorizontalHeaderItem(col++, new QTableWidgetItem(tab.name.c_str()));
        else
            col++;

        widget.setColumnWidth(col - 1, tab.size);
    }
}

void Table_View::do_resize()
{
    setup_view(config_, *widget_, true);
}

void Table_View::close_view()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    emit closing();
}

std::vector<settings::Tab_Configuration> Table_View::get_view_configuration()
{
    return config_;
}

}}
