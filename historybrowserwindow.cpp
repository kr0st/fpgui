#include <QClipboard>

#include "historybrowserwindow.h"
#include "ui_historybrowserwindow.h"
#include <utils.h>
#include <settings.h>

#include <set>

HistoryBrowserWindow::HistoryBrowserWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryBrowserWindow)
{
    ui->setupUi(this);

    ui->from_datetime->setDateTime(QDateTime::currentDateTimeUtc());
    ui->to_datetime->setDateTime(QDateTime::currentDateTimeUtc());

    QSettings settings;

    auto conf = fpgui::settings::read_app_config(settings);
    ui->per_page_edit->setText(std::to_string(conf.view_max_messages).c_str());

    ui->per_page_edit->setValidator(new QIntValidator(1, 10000, this));
    ui->goto_edit->setValidator(new QIntValidator(1, 65534, this));

    this->installEventFilter(&key_emitter_);
    connect(&key_emitter_, SIGNAL(key_pressed(QKeyEvent)), this, SLOT(on_key_press(QKeyEvent)));
    ui->tableWidget->addAction(ui->actionCopy);
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

void HistoryBrowserWindow::on_left_button_clicked()
{
    if (history_browser_view_)
        history_browser_view_->on_browse_back();
}

void HistoryBrowserWindow::on_right_button_clicked()
{
    if (history_browser_view_)
        history_browser_view_->on_browse_forward();
}

void HistoryBrowserWindow::on_per_page_edit_editingFinished()
{
    if (history_browser_view_)
        history_browser_view_->on_per_page_changed(ui->per_page_edit->text().toInt());
}

void HistoryBrowserWindow::on_goto_edit_editingFinished()
{
    if (history_browser_view_)
        history_browser_view_->on_goto_page(ui->goto_edit->text().toInt());
}

void HistoryBrowserWindow::on_page_counter_update(int current_page, int total_pages)
{
    std::string page_counter;
    page_counter = std::to_string(current_page) + "/" + std::to_string(total_pages);
    ui->page_counter_label->setText(page_counter.c_str());
}

int HistoryBrowserWindow::get_per_page_count()
{
    return ui->per_page_edit->text().toInt();
}

void HistoryBrowserWindow::on_tableWidget_itemActivated(QTableWidgetItem *item)
{
    if (history_browser_view_)
        history_browser_view_->on_item_activated(item->row());
}

void HistoryBrowserWindow::on_key_press(QKeyEvent e)
{
    if (e.key() == Qt::Key_Return)
    {
        if (ui->tableWidget->selectedItems().size() > 0)
        {
            QTableWidgetItem* item = ui->tableWidget->selectedItems()[0];
            if (history_browser_view_)
                history_browser_view_->on_item_activated(item->row());
        }
    }
}

void HistoryBrowserWindow::on_actionCopy_triggered()
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
        sz = (int)rows.size();
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
