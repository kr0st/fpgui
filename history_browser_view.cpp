#include "history_browser_view.h"
#include "historybrowserwindow.h"
#include "qpushbutton.h"

namespace fpgui { namespace ui {

History_Browser_View::History_Browser_View(settings::App_Configuration& app_config):
Table_View(app_config)
{
    app_config_.view_sorting = true;
    app_config_.view_autoscroll = false;
}

void History_Browser_View::on_history_browse()
{
    emit history_browse();
}

void History_Browser_View::on_datetime_changed(qint64 start_datetime, qint64 end_datetime)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    if (connected_)
    {
        QPushButton* stop_resume = widget_->parent()->findChild<QPushButton*>("connection_button");

        if (stop_resume)
            stop_resume->setText(tr("Resume"));

        connected_ = false;
    }

    emit datetime_changed(start_datetime, end_datetime);
}

void History_Browser_View::on_update_page_counter(int current_page, int total_pages)
{
    HistoryBrowserWindow* wnd = dynamic_cast<HistoryBrowserWindow*>(window_);
    wnd->on_page_counter_update(current_page, total_pages);
}

}}
