#include "history_browser_controller.h"

History_Browser_Controller::History_Browser_Controller(fpgui::ui::History_Browser_View &view):
Table_Controller(view)
{
    stop_when_no_data_ = true;

    connect(&view_, SIGNAL(history_browse()), this, SLOT(on_history_browse()));
    connect(&view_, SIGNAL(datetime_changed(qint64, qint64)), this, SLOT(on_datetime_changed(qint64, qint64)));

    connect(&view_, SIGNAL(browse_forward()), this, SLOT(on_browse_forward()));
    connect(&view_, SIGNAL(browse_back()), this, SLOT(on_browse_back()));
    connect(&view_, SIGNAL(clear_view()), this, SLOT(on_clear_screen()));

    connect(this, SIGNAL(page_counter_update(int,int)), &view, SLOT(on_update_page_counter(int, int)));
}

void History_Browser_Controller::on_history_browse()
{
    prev_page_ = -1;
    current_page_ = 0;
    total_pages_ = 0;

    QSettings settings;
    per_page_ = fpgui::settings::read_app_config(settings).view_max_messages;

    per_page_ = 3;

    emit page_counter_update(current_page_, total_pages_);

    view_.show();
}

void History_Browser_Controller::on_datetime_changed(qint64 start_datetime, qint64 end_datetime)
{
    stop_refreshing_view();
    view_.clear_screen();

    prev_page_ = -1;
    current_page_ = 0;
    total_pages_ = 0;

    emit page_counter_update(current_page_, total_pages_);

    QSettings settings;
    per_page_ = fpgui::settings::read_app_config(settings).view_max_messages;

    per_page_ = 3;

    std::map<QVariant, QVariant> options;
    options["time_start"] = start_datetime;
    options["time_end"] = end_datetime;

    data_source_->configure(options);
}

void History_Browser_Controller::request_data(std::queue<std::string>& data)
{
    if (data_source_.get() && (prev_page_ != current_page_))
    {
        total_pages_ = data_source_->request_paged_data(current_page_, per_page_, data);
        prev_page_ = current_page_;
    }
    else
        return;

    emit page_counter_update(current_page_ + 1, total_pages_);
}

void History_Browser_Controller::on_browse_back()
{
    if (current_page_ <= 0)
        return;

    current_page_--;

    view_.clear_screen();
    start_refreshing_view();
}

void History_Browser_Controller::on_browse_forward()
{
    if (current_page_ >= (total_pages_ - 1))
        return;

    current_page_++;

    view_.clear_screen();
    start_refreshing_view();
}

void History_Browser_Controller::on_clear_screen()
{
    prev_page_ = -1;
    current_page_ = 0;
    total_pages_ = 0;

    emit page_counter_update(current_page_, total_pages_);
}
