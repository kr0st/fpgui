#include <preferences.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSignalBlocker>
#include <QClipboard>
#include <qlist.h>
#include <utils.h>

#include <set>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    table_view_ = nullptr;
    ui->setupUi(this);

    this->installEventFilter(&key_emitter_);
    connect(&key_emitter_, SIGNAL(key_pressed(QKeyEvent)), this, SLOT(on_key_press(QKeyEvent)));
    ui->tableWidget->addAction(ui->actionCopy);
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

void MainWindow::message_box(const QString &text)
{
    generic_utils::ui::message_box(text);
}

void MainWindow::on_actionBrowse_triggered()
{
    if (history_browser_view_)
        history_browser_view_->on_history_browse();
}

void MainWindow::on_tableWidget_itemActivated(QTableWidgetItem *item)
{
    if (table_view_)
        table_view_->on_item_activated(item->row());
}

void MainWindow::on_key_press(QKeyEvent e)
{
    if (e.key() == Qt::Key_Return)
    {
        if (ui->tableWidget->selectedItems().size() > 0)
        {
            QTableWidgetItem* item = ui->tableWidget->selectedItems()[0];
            if (table_view_)
                table_view_->on_item_activated(item->row());
        }
    }
}

void MainWindow::on_actionCopy_triggered()
{
    int sz = ui->tableWidget->selectedItems().size();
    std::set<int> rows;
    int min_row = 100000000;

    for (int i = 0; i < sz; ++i)
    {
        QTableWidgetItem* item = ui->tableWidget->selectedItems()[i];
        int row(item->row());

        if (row < min_row)
            min_row = row;

        rows.insert(row);
    }

    if (rows.size() > 0)
    {
        sz = static_cast<int>(rows.size());
        QString result("");

        for (int i = 0; i < sz; ++i)
        {
            for (int j = 0; j < ui->tableWidget->columnCount(); ++j)
            {
                QTableWidgetItem* item = ui->tableWidget->item(i + min_row, j);
                if (item)
                    result += (item->text() + "|");
                else
                    result += ("|");
            }
            result += "\n";
        }

        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(result);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    Preferences settings;
    settings.exec();
}
