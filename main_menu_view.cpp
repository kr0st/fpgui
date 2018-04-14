#include "main_menu_view.h"
#include "qpushbutton.h"

namespace fpgui { namespace ui {

Main_Menu_View::Main_Menu_View(settings::App_Configuration& app_config):
Table_View(app_config)
{
    app_config_.view_sorting = true;
    app_config_.view_autoscroll = false;
}

void Main_Menu_View::on_history_browse()
{
    emit history_browse();
}

void Main_Menu_View::on_datetime_changed(qint64 start_datetime, qint64 end_datetime)
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

}}
