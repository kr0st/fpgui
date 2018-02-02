#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSignalBlocker>
#include <utils.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    table_view_ = 0;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   if (table_view_)
       table_view_->do_resize();
}

void MainWindow::on_autoscroll_box_stateChanged(int state)
{
   if (table_view_)
       table_view_->on_autoscroll_change(state);
}

void MainWindow::on_sorting_box_stateChanged(int state)
{
    if (table_view_)
        table_view_->on_sorting_change(state);
}

void MainWindow::on_clear_button_clicked()
{
    if (table_view_)
        table_view_->on_clear_screen();
}

void MainWindow::on_connection_button_clicked()
{
    if (table_view_)
        table_view_->on_connection_stop_resume();
}

void MainWindow::on_quickfilter_edit_textEdited(const QString &text)
{
    if (table_view_)
        table_view_->on_quick_filter(text);
}

void MainWindow::message_box(QString &text)
{
    generic_utils::ui::message_box(text);
}
