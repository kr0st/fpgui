#include <algorithm>
#include <iostream>

#include "table_controller.h"
#include <utils.h>
#include <scripting.h>
#include <var_injector.h>

#include <QTimer>
#include <QThread>
#include <QCheckBox>
#include <QtDebug>

namespace fpgui {
namespace ui {

class Table_Controller::Timer_Thread: public QThread
{
    public:

        Timer_Thread(Table_Controller& controller):
        controller_(controller)
        {
        }


    private:

        Table_Controller& controller_;

        void run()
        {
            controller_.refresh_view_internal();
            exec();
        }
};

Table_Controller::~Table_Controller()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    stop_refreshing_view();
}

Table_Controller::Table_Controller(Table_View& view):
view_(view),
data_source_(0)
{
    connect(&view, SIGNAL(closing()), this, SLOT(on_view_closing()), Qt::DirectConnection);
    connect(&view, SIGNAL(autoscroll_change(int)), this, SLOT(on_autoscroll_change(int)), Qt::DirectConnection);
    connect(&view, SIGNAL(sorting_change(int)), this, SLOT(on_sorting_change(int)), Qt::DirectConnection);
    connect(&view, SIGNAL(clear_view()), this, SLOT(on_clear_screen()), Qt::DirectConnection);
    connect(&view, SIGNAL(stop_resume()), this, SLOT(on_connection_stop_resume()), Qt::DirectConnection);

    QSettings settings;
    app_config_ = settings::read_app_config(settings);
    tab_config_ = settings::read_tab_config(settings);

    timer_thread_.reset(new Timer_Thread(*this));
}

void Table_Controller::stop_refreshing_view()
{
    bool is_running = true;

    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        is_running = timer_thread_->isRunning();
        if (is_running)
            timer_thread_->quit();
    }

    if (is_running)
        timer_thread_->wait();
}

void Table_Controller::merge_view_config(const Table_View::View_Configuration& config)
{
    size_t len = config.tab_config.size(), this_len = tab_config_.size();

    if (len != this_len)
        qWarning() << "Trying to merge view config that has different size, some elemtns might be misconfigured.";

    if (len > this_len)
        len = this_len;

    for (size_t i = 0; i < len; ++i)
    {
        tab_config_[i].size = config.tab_config[i].size;
    }

    app_config_.window_height = config.app_config.window_height;
    app_config_.window_width = config.app_config.window_width;
}

void Table_Controller::on_view_closing()
{
    stop_refreshing_view();
    merge_view_config(view_.get_view_configuration());

    QSettings settings;

    settings::write_tab_config(tab_config_, settings);
    settings::write_app_config(app_config_, settings);
}

static void prepare_for_display(std::vector<std::string>& json_strings, std::vector<settings::Tab_Configuration>& tab_config)
{
    std::string to_show;
    for (auto& tab : tab_config)
        if (tab.show) to_show += (tab.name + ",");

    json_strings = generic_utils::strip_json(to_show, json_strings);
}

static std::vector<std::pair<const std::string*, const std::string*>> sort_batch(const std::vector<std::string>& batch,
                                                                      std::vector<settings::Tab_Configuration>& tab_config,
                                                                      settings::App_Configuration& app_config)
{
    std::string sort_by;
    for (auto& tab : tab_config)
        if (tab.sort_by) sort_by += (tab.name + ",");

    std::vector<std::string> stripped(generic_utils::strip_json(sort_by, batch));
    std::vector<std::pair<const std::string*, const std::string*>> to_sort;

    for (size_t i = 0; i < batch.size(); ++i)
        to_sort.push_back(std::pair<const std::string*, const std::string*>(&batch[i], &stripped[i]));

    if (app_config.view_sorting)
    {
        std::sort(to_sort.begin(), to_sort.end(), [](const std::pair<const std::string*, const std::string*>& s1, const std::pair<const std::string*, const std::string*>& s2) {
            int res = fpgui::lua::compare_json_strings(*s1.second, *s2.second);
            if ((res < -1) || (res == 0))
                return false;
            if (res == -1)
                return true;
            return false;
        });
    }

    return to_sort;
}

static void trim_data(std::vector<std::string>& data, settings::App_Configuration& config)
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
            for (size_t i = 0; i < (upper_limit / (size_t)config.view_clearing_ratio); ++i)
                data.erase(data.begin());
        }
    #endif
}

void Table_Controller::start_refreshing_view()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!timer_thread_->isRunning())
    {
        timer_thread_->start();
        if (QThread::currentThread() != timer_thread_.get())
            this->moveToThread(timer_thread_.get());
    }
}

void Table_Controller::refresh_view_internal()
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
                auto sorted(sort_batch(batch, tab_config_, app_config_));
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
        auto sorted(sort_batch(batch, tab_config_, app_config_));
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

    QTimer::singleShot(app_config_.view_refresh_time, this, SLOT(refresh_view_internal()));
}

void Table_Controller::on_autoscroll_change(int state)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    app_config_.view_autoscroll = (state == Qt::Checked ? true : false);
}

void Table_Controller::on_sorting_change(int state)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    app_config_.view_sorting = (state == Qt::Checked ? true : false);
}

void Table_Controller::on_clear_screen()
{
    bool refreshing = timer_thread_->isRunning();

    if (refreshing)
        stop_refreshing_view();

    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        data_.clear();
        display_data_.clear();
    }

    view_.clear_screen();

    if (refreshing)
        start_refreshing_view();
}

void Table_Controller::on_connection_stop_resume()
{
    bool is_running = true;

    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        is_running = timer_thread_->isRunning();
    }

    if (is_running)
        stop_refreshing_view();
    else
        start_refreshing_view();
}

}
}
