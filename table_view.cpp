#include "table_view.h"
#include <utils.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <QHeaderView>
#include <QDebug>

#include <vector>


namespace fpgui {
namespace ui {

static void balance_size_percentages(std::vector<settings::Tab_Configuration>& config, double total = 0)
{
    int invisible = 0;

    for (auto& tab : config)
        if (!tab.show)
            invisible++;

    double min_tab_size = 100 / (config.size() - invisible);

    if (min_tab_size > 5)
        min_tab_size = 5;

    if (total < 0)
        total = 0;

    if (total < 0.1)
    {
        for (auto& tab : config)
        {
            if (!tab.show)
                continue;

            if (tab.size < min_tab_size)
                tab.size = min_tab_size;
            total += tab.size;
        }

        if ((total < 101) && (total > 99))
            return;

        balance_size_percentages(config, total);
        return;
    }

    while (total > 101)
    {
        double temp = 0;

        for (auto& tab : config)
        {
            if (!tab.show)
                continue;

            tab.size -= 0.1;

            if (tab.size < min_tab_size)
                tab.size = min_tab_size;

            temp += tab.size;
        }

        total = temp;
    }

    while (total < 99)
    {
        double temp = 0;

        for (auto& tab : config)
        {
            if (!tab.show)
                continue;

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
    {
        if (!tab.show)
            continue;

        tab.size *= (widget_width / 100);
    }
}

static void absolute_width_to_percentage(std::vector<settings::Tab_Configuration> &config, QTableWidget &widget)
{
    double widget_width(widget.geometry().width());

    int col = 0;

    for (auto& tab : config)
    {
        if (!tab.show)
            continue;

        double width = widget.columnWidth(col++);
        tab.size = width / widget_width * 100;
    }
}

static bool suppress_resize_signals = false;

void Table_View::setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget &widget, bool resize_only)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    generic_utils::Variable_Reset <bool> reset(suppress_resize_signals, true, false);

    double widget_width(widget.geometry().width());
    std::vector<settings::Tab_Configuration> config_copy(config);

    if (!resize_only)
    {
        balance_size_percentages(config_copy);

        config_ = config_copy;
        widget_ = &widget;

        int invisible = 0;

        for (auto& tab : config)
            if (!tab.show)
                invisible++;

        widget.setColumnCount(config_copy.size() - invisible);
        widget.setRowCount(1);
        widget.setSelectionBehavior(QAbstractItemView::SelectRows);
        widget.setSelectionMode(QAbstractItemView::SingleSelection);

        disconnect(widget_->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this,
                   SLOT(col_size_changed(int, int, int)));

        connect(widget_->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this,
                SLOT(col_size_changed(int, int, int)), Qt::DirectConnection);

        int col = 0;
        for (auto& tab : config_copy)
        {
            if (!tab.show)
                continue;

            widget.setHorizontalHeaderItem(col++, new QTableWidgetItem(tab.name.c_str()));
        }
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
        if (!tab.show)
            continue;

        widget.setColumnWidth(col++, tab.size);
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

void Table_View::col_size_changed(int, int, int)
{
    if (!suppress_resize_signals)
        do_resize();
}

static void trim_data(std::vector<std::string>& data, settings::App_Configuration& config, QTableWidget* widget = 0)
{
    size_t upper_limit = (size_t)config.view_max_messages;

    #ifdef _UNIT_TEST
        upper_limit = 650;
        while (data.size() > upper_limit)
            data.erase(data.begin());
    #endif

    #ifndef _UNIT_TEST
        if (data.size() > upper_limit)
        {
            size_t remove_count = (upper_limit / (size_t)config.view_clearing_ratio);
            for (size_t i = 0; i < remove_count; ++i)
            {
                if (widget)
                    widget->removeRow(remove_count - i - 1);
                data.erase(data.begin());
            }
        }
    #endif
}

void Table_View::refresh_view(std::vector<std::string>& data_batch, bool)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    data_.reserve(data_.size() + data_batch.size());
    std::copy(data_batch.begin(), data_batch.end(), std::inserter(data_, data_.end()));

    #ifndef _UNIT_TEST
        display_strings(data_batch);
        trim_data(data_, app_config_, widget_);
    #else
        trim_data(data_, app_config_, 0);
    #endif
}

void Table_View::display_strings(std::vector<std::string> &json_strings)
{
    for (const auto& js: json_strings)
    {
        rapidjson::GenericDocument<rapidjson::UTF8<>> js_from;
        std::unique_ptr<char[]> to_parse(new char[js.size() + 1]);

        memcpy(to_parse.get(), js.c_str(), js.size());
        to_parse.get()[js.size()] = 0;

        js_from.ParseInsitu(to_parse.get());

        if (js_from.IsNull())
        {
            qCritical() << "JSON document is invalid!";
            return;
        }

        std::map<std::string, int> col_name_to_number;
        for (int i = 0; i < widget_->horizontalHeader()->count(); ++i)
        {
            std::pair<std::string, int> pair(widget_->horizontalHeaderItem(i)->text().toStdString(), i);
            col_name_to_number.insert(pair);
        }

        if (js_from.IsObject())
        {
            rapidjson::Value::Object jsobj(js_from.GetObject());

            for (rapidjson::Value::Object::MemberIterator it = jsobj.MemberBegin(); it != jsobj.MemberEnd(); ++it)
            {
                auto item(col_name_to_number.find(it->name.GetString()));
                if (item == col_name_to_number.end())
                    continue;

                widget_->setItem(widget_->rowCount() - 1, item->second, new QTableWidgetItem(it->value.GetString()));
            }

            widget_->insertRow(widget_->rowCount());
        }
    }

}

}}
