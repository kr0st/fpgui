#include "history_browser_controller.h"

History_Browser_Controller::History_Browser_Controller(fpgui::ui::History_Browser_View &view):
Table_Controller(view)
{
    stop_when_no_data_ = true;

    connect(&view_, SIGNAL(history_browse()), this, SLOT(on_history_browse()));
    connect(&view_, SIGNAL(datetime_changed(qint64, qint64)), this, SLOT(on_datetime_changed(qint64, qint64)));
}

void History_Browser_Controller::on_history_browse()
{
    view_.show();
}

void History_Browser_Controller::on_datetime_changed(qint64 start_datetime, qint64 end_datetime)
{
    stop_refreshing_view();
    view_.clear_screen();

    std::map<QVariant, QVariant> options;
    options["time_start"] = start_datetime;
    options["time_end"] = end_datetime;

    data_source_->configure(options);
}
