#include <algorithm>

#include "table_controller.h"
#include <utils.h>
#include <scripting.h>
#include <var_injector.h>

#include <QTimer>

namespace fpgui {
namespace ui {

Table_Controller::Table_Controller(Table_View& view):
view_(view),
view_is_refreshing_(true),
data_source_(0)
{
    connect(&view, SIGNAL(closing()), this, SLOT(on_view_closing()), Qt::DirectConnection);

    QSettings settings;
    app_config_ = settings::read_app_config(settings);
    tab_config_ = settings::read_tab_config(settings);
}

void Table_Controller::stop_refreshing_view()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    view_is_refreshing_ = false;
}

void Table_Controller::on_view_closing()
{
    QSettings settings;
    settings::write_tab_config(view_.get_view_configuration(), settings);
}

static void prepare_for_display(std::vector<std::string>& json_strings, std::vector<settings::Tab_Configuration>& tab_config)
{
    std::string to_show;
    for (auto& tab : tab_config)
        if (tab.show) to_show += (tab.name + ",");

    json_strings = generic_utils::strip_json(to_show, json_strings);
}

static std::vector<std::pair<const std::string*, const std::string*>> sort_batch(const std::vector<std::string>& batch,
                                                                     std::vector<settings::Tab_Configuration>& tab_config)
{
    std::string sort_by;
    for (auto& tab : tab_config)
        if (tab.sort_by) sort_by += (tab.name + ",");

    std::vector<std::string> stripped(generic_utils::strip_json(sort_by, batch));
    std::vector<std::pair<const std::string*, const std::string*>> to_sort;

    for (size_t i = 0; i < batch.size(); ++i)
        to_sort.push_back(std::pair<const std::string*, const std::string*>(&batch[i], &stripped[i]));

    std::sort(to_sort.begin(), to_sort.end(), [](const std::pair<const std::string*, const std::string*>& s1, const std::pair<const std::string*, const std::string*>& s2) {
        int res = fpgui::lua::compare_json_strings(*s1.second, *s2.second);
        if ((res < -1) || (res == 0))
            return false;
        if (res == -1)
            return true;
        return false;
    });

    return to_sort;
}

static void trim_data(std::vector<std::string>& data, settings::App_Configuration& config)
{
    while (data.size() > (size_t)config.view_max_messages)
        data.erase(data.begin());
}

void Table_Controller::refresh_view()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    fpgui::lua::inject_tab_sorting_config(tab_config_);

    std::queue<std::string> data;
    if (data_source_.get())
        data_source_->request_data(data);

    std::vector<std::string> batch;

    if (!data.empty())
        do
        {
            batch.push_back(data.front());
            data.pop();
            if (batch.size() % app_config_.view_batch_size == 0)
            {
                auto sorted(sort_batch(batch, tab_config_));
                std::vector<std::string> display_batch;

                for (auto& str : sorted)
                {
                    display_batch.push_back(*str.first);
                    data_.push_back(*str.first);
                }

                prepare_for_display(display_batch, tab_config_);

                display_data_.reserve(display_data_.size() + display_batch.size());
                view_.refresh_view(display_batch);
                std::move(display_batch.begin(), display_batch.end(), std::inserter(display_data_, display_data_.end()));

                batch.resize(0);
            }
        } while (!data.empty());

    if (batch.size())
    {
        auto sorted(sort_batch(batch, tab_config_));
        std::vector<std::string> display_batch;

        for (auto& str : sorted)
        {
            display_batch.push_back(*str.first);
            data_.push_back(*str.first);
        }

        prepare_for_display(display_batch, tab_config_);

        display_data_.reserve(display_data_.size() + display_batch.size());
        view_.refresh_view(display_batch);
        std::move(display_batch.begin(), display_batch.end(), std::inserter(display_data_, display_data_.end()));
    }

    trim_data(data_, app_config_);
    trim_data(display_data_, app_config_);

    QTimer::singleShot(app_config_.view_refresh_time, this, SLOT(refresh_view()));
}

}
}
