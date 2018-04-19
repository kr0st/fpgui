#include "historybrowserwindow.h"
#include "ui_historybrowserwindow.h"
#include <utils.h>

HistoryBrowserWindow::HistoryBrowserWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryBrowserWindow)
{
    ui->setupUi(this);

    ui->from_datetime->setDateTime(QDateTime::currentDateTimeUtc());
    ui->to_datetime->setDateTime(QDateTime::currentDateTimeUtc());
}

HistoryBrowserWindow::~HistoryBrowserWindow()
{
    delete ui;
}

void HistoryBrowserWindow::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);

   if (history_browser_view_)
       history_browser_view_->do_resize();
}

void HistoryBrowserWindow::on_autoscroll_box_stateChanged(int state)
{
   if (history_browser_view_)
       history_browser_view_->on_autoscroll_change(state);
}

void HistoryBrowserWindow::on_sorting_box_stateChanged(int state)
{
    if (history_browser_view_)
        history_browser_view_->on_sorting_change(state);
}

void HistoryBrowserWindow::on_clear_button_clicked()
{
    if (history_browser_view_)
        history_browser_view_->on_clear_screen();
}

void HistoryBrowserWindow::on_connection_button_clicked()
{
    if (history_browser_view_)
        history_browser_view_->on_connection_stop_resume();
}

void HistoryBrowserWindow::on_quickfilter_edit_textEdited(const QString &text)
{
    if (history_browser_view_)
        history_browser_view_->on_quick_filter(text);
}

void HistoryBrowserWindow::message_box(const QString &text)
{
    generic_utils::ui::message_box(text);
}

void HistoryBrowserWindow::on_from_datetime_editingFinished()
{
    qint64 from = ui->from_datetime->dateTime().toSecsSinceEpoch(),
           to = ui->to_datetime->dateTime().toSecsSinceEpoch();
    if (history_browser_view_)
        history_browser_view_->on_datetime_changed(from, to);
}

void HistoryBrowserWindow::on_to_datetime_editingFinished()
{
    qint64 from = ui->from_datetime->dateTime().toSecsSinceEpoch(),
           to = ui->to_datetime->dateTime().toSecsSinceEpoch();
    if (history_browser_view_)
        history_browser_view_->on_datetime_changed(from, to);
}
