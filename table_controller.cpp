#include <algorithm>

#include "table_controller.h"
#include <utils.h>
#include <scripting.h>

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

void Table_Controller::refresh_view()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

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
                for (auto& str : sorted)
                    data_.push_back(*str.first); //TODO: here push also into table view

                batch.resize(0);
            }
        } while (!data.empty());

    if (batch.size())
    {
        auto sorted(sort_batch(batch, tab_config_));
        for (auto& str : sorted)
            data_.push_back(*str.first); //TODO: here push also into table view
    }

    QTimer::singleShot(app_config_.view_refresh_time, this, SLOT(refresh_view()));
}

}
}
