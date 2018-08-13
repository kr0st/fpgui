#include <algorithm>
#include <iostream>

#include <table_controller.h>
#include <utils.h>
#include <scripting.h>
#include <var_injector.h>
#include <mongo_data_source.h>
#include <settings.h>
#include <globals.h>
#include <message_details_dialog.h>

#include <QTimer>
#include <QThread>
#include <QCheckBox>
#include <QtDebug>
#include <QApplication>

namespace fpgui {
namespace ui {

Table_Controller::~Table_Controller()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    stop_refreshing_view();
    event_loop_.quit();
    event_loop_.wait();
}

Table_Controller::Table_Controller(Table_View& view):
view_(view),
data_source_(0),
stop_when_no_data_(false),
prev_page_(0),
current_page_(0),
total_pages_(0),
per_page_(0)
{
    connect(&view, SIGNAL(closing()), this, SLOT(on_view_closing()), Qt::DirectConnection);
    connect(&view, SIGNAL(autoscroll_change(int)), this, SLOT(on_autoscroll_change(int)), Qt::DirectConnection);
    connect(&view, SIGNAL(sorting_change(int)), this, SLOT(on_sorting_change(int)), Qt::DirectConnection);
    connect(&view, SIGNAL(clear_view()), this, SLOT(on_clear_screen()), Qt::DirectConnection);
    connect(&view, SIGNAL(stop_resume()), this, SLOT(on_connection_stop_resume()), Qt::DirectConnection);
    connect(&view, SIGNAL(item_activated(int)), this, SLOT(item_activated(int)));
    connect(this, SIGNAL(display_details(QString)), &view, SLOT(display_details(QString)), Qt::QueuedConnection);

    qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
    connect(this, SIGNAL(refresh_view(std::vector<std::string>,bool)), &view, SLOT(refresh_view(std::vector<std::string>,bool)));

    QSettings settings;
    app_config_ = settings::read_app_config(settings);
    tab_config_ = settings::read_tab_config(settings);

    is_running_ = false;
    event_loop_.start();

    if (this->thread() != &event_loop_)
        this->moveToThread(&event_loop_);
}

void Table_Controller::stop_refreshing_view()
{
    is_running_ = false;

    try
    {
        if (data_source_.get())
            data_source_->disconnect();
    }
    catch (fpgui::exceptions::Generic_Exception& e)
    {
        view_.display_message(e.what());
    }
    catch (mongocxx::exception& e)
    {
        view_.display_message(e.what());
    }
}

void Table_Controller::merge_view_config(const Table_View::View_Configuration& config)
{
    size_t len = config.tab_config.size(), this_len = tab_config_.size();

    if (len != this_len)
        qWarning() << tr("Trying to merge view config that has different size, some elements might be misconfigured.");

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

static bool trim_data(std::vector<std::string>& data, settings::App_Configuration& config)
{
    size_t upper_limit = (size_t)config.view_max_messages;
    bool trimmed = false;

    #ifdef _UNIT_TEST
        upper_limit = 650;
        while (data.size() > upper_limit)
        {
            data.erase(data.begin());
            trimmed = true;
        }
    #endif

    #ifndef _UNIT_TEST
        if (data.size() > upper_limit)
        {
            for (size_t i = 0; i < (upper_limit / (size_t)config.view_clearing_ratio); ++i)
            {
                data.erase(data.begin());
                trimmed = true;
            }
        }
    #endif

    return trimmed;
}

void Table_Controller::start_refreshing_view()
{
    try
    {
        QSettings settings;
        if (data_source_.get())
            data_source_->connect(settings);
        else
            THROWM(fpgui::exceptions::Incorrect_Parameter, tr("Cannot establish data source connection! Please re-launch the application."));
    }
    catch (fpgui::exceptions::Generic_Exception& e)
    {
        view_.display_message(e.what());
        view_.reset_connected_state();
        return;
    }
    catch (mongocxx::exception& e)
    {
        view_.display_message(e.what());
        view_.reset_connected_state();
        return;
    }

    is_running_ = true;
    refresh_view_internal();
}

void Table_Controller::request_data(std::queue<std::string>& data)
{
    if (data_source_.get())
        data_source_->request_data(data);
}

void Table_Controller::refresh_view_internal()
{
    if (!is_running_)
        return;

    if (this->thread() != &event_loop_)
        this->moveToThread(&event_loop_);

    std::queue<std::string> data;

    try
    {
        request_data(data);

        if (data.empty() && stop_when_no_data_)
        {
            stop_refreshing_view();
            view_.reset_connected_state();
        }
    }
    catch (fpgui::exceptions::Generic_Exception& e)
    {
        view_.display_message(e.what());
        stop_refreshing_view();
        view_.reset_connected_state();
        data_source_.reset();

        return;
    }
    catch (mongocxx::exception& e)
    {
        view_.display_message(e.what());
        stop_refreshing_view();
        view_.reset_connected_state();
        data_source_.reset();

        return;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    fpgui::lua::inject_tab_sorting_config(tab_config_);

    std::vector<std::string> batch;

    std::string mandatory_fields;

    mandatory_fields += (fpgui::settings::tab_names.facility + std::string(","));
    mandatory_fields += (fpgui::settings::tab_names.priority + std::string(","));
    mandatory_fields += (fpgui::settings::tab_names.timestamp + std::string(","));
    mandatory_fields += (fpgui::settings::tab_names.hostname + std::string(","));
    mandatory_fields += (fpgui::settings::tab_names.appname + std::string(","));
    mandatory_fields += fpgui::settings::tab_names.sequence;

    if (!data.empty())
        do
        {
            if (generic_utils::validate_json(mandatory_fields, data.front()))
                batch.push_back(data.front());
            else
            {
                data.pop();
                continue;
            }

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
                emit refresh_view(display_batch);
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
        emit refresh_view(display_batch);
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
    bool was_running = false;
    if (is_running_)
    {
        was_running = true;
        stop_refreshing_view();
    }

    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        data_.clear();
        display_data_.clear();
    }

    view_.clear_screen();

    if (was_running)
        start_refreshing_view();
}

void Table_Controller::on_connection_stop_resume()
{
    if (is_running_)
        stop_refreshing_view();
    else
        start_refreshing_view();
}

void Table_Controller::item_activated(int index)
{
    if (index < 0)
        return;

    size_t res = current_page_ * per_page_ + index;
    if (res < data_.size())
        emit display_details(data_[res].c_str());
}

}
}
