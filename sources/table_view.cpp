#include "table_view.h"
#include "mainwindow.h"
#include "historybrowserwindow.h"
#include <utils.h>
#include <windowwithmessagebox.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <message_details_dialog.h>

#include <QHeaderView>
#include <QDebug>
#include <QMainWindow>
#include <QCheckBox>
#include <QScrollBar>
#include <QPushButton>
#include <QApplication>

#include <vector>
#include <map>


namespace fpgui {
namespace ui {

static void balance_size_percentages(std::vector<settings::Tab_Configuration>& config, double total = 0)
{
    int invisible = 0;

    for (auto& tab : config)
        if (!tab.show)
            invisible++;

    double min_tab_size = 100 / (config.size() - invisible);

    if (min_tab_size > 5)
        min_tab_size = 5;

    if (total < 0)
        total = 0;

    if (total < 0.1)
    {
        for (auto& tab : config)
        {
            if (!tab.show)
                continue;

            if (tab.size < min_tab_size)
                tab.size = min_tab_size;
            total += tab.size;
        }

        if ((total < 101) && (total > 99))
            return;

        balance_size_percentages(config, total);
        return;
    }

    while (total > 101)
    {
        double temp = 0;

        for (auto& tab : config)
        {
            if (!tab.show)
                continue;

            tab.size -= 0.1;

            if (tab.size < min_tab_size)
                tab.size = min_tab_size;

            temp += tab.size;
        }

        total = temp;
    }

    while (total < 99)
    {
        double temp = 0;

        for (auto& tab : config)
        {
            if (!tab.show)
                continue;

            tab.size += 0.1;

            if (tab.size < min_tab_size)
                tab.size = min_tab_size;

            temp += tab.size;
        }

        total = temp;
    }
}

static void percentage_to_absolute_width(std::vector<settings::Tab_Configuration> &config, double widget_width)
{
    if (widget_width < 1)
        return;

    for (auto& tab : config)
    {
        if (!tab.show)
            continue;

        tab.size *= (widget_width / 100);
    }
}

static void absolute_width_to_percentage(std::vector<settings::Tab_Configuration> &config, QTableWidget &widget)
{
    double widget_width(widget.geometry().width());

    int col = 0;

    for (auto& tab : config)
    {
        if (!tab.show)
            continue;

        double width = widget.columnWidth(col++);
        tab.size = width / widget_width * 100;
    }
}

static bool suppress_resize_signals = false;

void Table_View::show()
{
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);

    QObject::connect(timer, &QTimer::timeout, [&]()
    {
        //Here we executing the action on the main/GUI thread
        //in order to get rid of errors on macOS, where updating GUI
        //from non-main thread is prohibited.
        if (window_)
        {
            QWidget* wnd = dynamic_cast<QWidget*>(window_);
            if (wnd->isHidden())
                wnd->show();
        }
        timer->deleteLater();
    });

    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}

void Table_View::hide()
{
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);

    QObject::connect(timer, &QTimer::timeout, [&]()
    {
        //Here we executing the action on the main/GUI thread
        //in order to get rid of errors on macOS, where updating GUI
        //from non-main thread is prohibited.
        if (window_)
        {
            QWidget* wnd = dynamic_cast<QWidget*>(window_);
            if (!wnd->isHidden())
                wnd->hide();
        }
        timer->deleteLater();
    });

    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}

void Table_View::show_hide()
{
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);

    QObject::connect(timer, &QTimer::timeout, [&]()
    {
        //Here we executing the action on the main/GUI thread
        //in order to get rid of errors on macOS, where updating GUI
        //from non-main thread is prohibited.
        if (window_)
        {
            QWidget* wnd = dynamic_cast<QWidget*>(window_);
            if (wnd->isHidden())
                wnd->show();
            else
                wnd->hide();
        }
        timer->deleteLater();
    });

    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}

void Table_View::setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget &widget, bool resize_only, WindowWithMessageBoxInterface *window)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    generic_utils::Variable_Reset <bool> reset(suppress_resize_signals, true, false);

    double widget_width(widget.geometry().width());
    std::vector<settings::Tab_Configuration> config_copy(config);

    if (!resize_only)
    {
        window_ = window;

        disconnect(widget.horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this,
                SLOT(col_size_changed(int, int, int)));
        disconnect(widget.model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), this,
                   SLOT(rows_inserted(const QModelIndex&, int, int)));

        int invisible = 0;

        for (auto& tab : config)
            if (!tab.show)
                invisible++;

        widget.setColumnCount(config_copy.size() - invisible);
        widget.setRowCount(1);
        widget.setSelectionBehavior(QAbstractItemView::SelectRows);
        widget.setSelectionMode(QAbstractItemView::ContiguousSelection);

        QWidget* wnd = dynamic_cast<QWidget*>(widget.parent()->parent());
        if (!wnd) wnd = dynamic_cast<QWidget*>(widget.parent());

        QObject::disconnect(wnd, SIGNAL(display_message(const QString &)), wnd, SLOT(message_box(const QString &)));
        QObject::connect(wnd, SIGNAL(display_message(const QString &)), wnd, SLOT(message_box(const QString &)), Qt::QueuedConnection);

        QCheckBox* autoscroll_box = wnd->findChild<QCheckBox*>("autoscroll_box");
        if (autoscroll_box)
        {
            autoscroll_box->blockSignals(true);
            autoscroll_box->setCheckState(app_config_.view_autoscroll ? Qt::Checked : Qt::Unchecked);
            autoscroll_box->blockSignals(false);
        }

        QCheckBox* sorting_box = wnd->findChild<QCheckBox*>("sorting_box");
        if (sorting_box)
        {
            sorting_box->blockSignals(true);
            sorting_box->setCheckState(app_config_.view_sorting ? Qt::Checked : Qt::Unchecked);
            sorting_box->blockSignals(false);
        }

        if ((app_config_.window_height != 0) && (app_config_.window_width != 0))
            wnd->resize(app_config_.window_width, app_config_.window_height);

        widget_width = widget.geometry().width();

        balance_size_percentages(config_copy);

        config_ = config_copy;
        widget_ = &widget;

        connect(widget_->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this,
                SLOT(col_size_changed(int, int, int)));
        connect(widget_->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), this,
                   SLOT(rows_inserted(const QModelIndex&, int, int)));

        int col = 0;
        for (auto& tab : config_copy)
        {
            if (!tab.show)
                continue;

            widget.setHorizontalHeaderItem(col++, new QTableWidgetItem(tab.name.c_str()));
        }
    }
    else
    {
        absolute_width_to_percentage(config_copy, widget);
        balance_size_percentages(config_copy);
        config_ = config_copy;

        QWidget* wnd = dynamic_cast<QWidget*>(widget_->parent()->parent());
        if (!wnd) wnd = dynamic_cast<QWidget*>(widget_->parent());

        app_config_.window_width = wnd->geometry().width();
        app_config_.window_height = wnd->geometry().height();
    }

    percentage_to_absolute_width(config_copy, widget_width);

    int col = 0;
    for (auto& tab : config_copy)
    {
        if (!tab.show)
            continue;

        widget.setColumnWidth(col++, tab.size);
    }
}

void Table_View::do_resize()
{
    setup_view(config_, *widget_, true);
}

void Table_View::close_view()
{
    emit closing();
}

Table_View::View_Configuration Table_View::get_view_configuration()
{
    View_Configuration config;

    config.app_config = app_config_;
    config.tab_config = config_;

    return config;
}

void Table_View::col_size_changed(int, int, int)
{
    if (!suppress_resize_signals)
        do_resize();
}

void Table_View::trim_view(bool clear_screen)
{
    size_t upper_limit = (size_t)app_config_.view_max_messages;

    #ifdef _UNIT_TEST
        upper_limit = 650;
        while (data_.size() > upper_limit)
            data_.erase(data_.begin());
    #endif

    #ifndef _UNIT_TEST
        if ((data_.size() >= upper_limit) || clear_screen)
        {
            size_t remove_count = (upper_limit / (size_t)app_config_.view_clearing_ratio);

            if (clear_screen)
                remove_count = data_.size();

            for (size_t i = 0; i < remove_count; ++i)
                data_.erase(data_.begin());

            if (widget_)
            {
                widget_->clearSelection();
                widget_->clearContents();
                widget_->setRowCount(1);
                display_strings(data_);
            }
        }
    #endif
}

void Table_View::refresh_view(std::vector<std::string> data_batch, bool)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);

    data_.reserve(data_.size() + data_batch.size());
    std::copy(data_batch.begin(), data_batch.end(), std::inserter(data_, data_.end()));

    #ifndef _UNIT_TEST
        display_strings(data_batch);
        apply_quick_filter();
    #endif

    trim_view();
}

void Table_View::display_strings(std::vector<std::string> &json_strings)
{
    for (const auto& js: json_strings)
    {
        rapidjson::GenericDocument<rapidjson::UTF8<>> js_from;
        std::unique_ptr<char[]> to_parse(new char[js.size() + 1]);

        memcpy(to_parse.get(), js.c_str(), js.size());
        to_parse.get()[js.size()] = 0;

        js_from.ParseInsitu(to_parse.get());

        if (js_from.IsNull())
        {
            qCritical() << tr("JSON document is invalid!");
            return;
        }

        std::map<std::string, int> col_name_to_number;
        for (int i = 0; i < widget_->horizontalHeader()->count(); ++i)
        {
            std::pair<std::string, int> pair(widget_->horizontalHeaderItem(i)->text().toStdString(), i);
            col_name_to_number.insert(pair);
        }

        if (js_from.IsObject())
        {
            rapidjson::Value::Object jsobj(js_from.GetObject());

            generic_utils::ui::Hsv_Rgb_Converter::rgb highlight(1, 1, 1);

            QColor text_color;
            bool text_bold = false;

            std::map<std::string, std::string> field_values;

            for (rapidjson::Value::Object::MemberIterator it = jsobj.MemberBegin(); it != jsobj.MemberEnd(); ++it)
            {
                auto item(col_name_to_number.find(it->name.GetString()));
                if (item == col_name_to_number.end())
                    continue;

                if (app_config_.highlighting.value_based_enabled)
                    for (auto config_item : app_config_.highlighting.config)
                        if (config_item.field.compare(it->name.GetString()) == 0)
                            if (config_item.value.compare(it->value.GetString()) == 0)
                            {
                                text_bold = config_item.bold;
                                text_color = config_item.color;

                                break;
                            }

                if (app_config_.highlighting.diff_enabled)
                {
                    std::string hostname("hostname");
                    if (hostname.compare(it->name.GetString()) == 0)
                        highlight = colorizer_.colorize(it->value.GetString());
                }

                widget_->setItem(widget_->rowCount() - 1, item->second, new QTableWidgetItem(it->value.GetString()));
            }

            if (app_config_.highlighting.diff_enabled || app_config_.highlighting.value_based_enabled)
                for (int i = 0; i < widget_->columnCount(); ++i)
                {
                    QTableWidgetItem* item = widget_->item(widget_->rowCount() - 1, i);
                    if (item)
                    {
                        if (app_config_.highlighting.diff_enabled)
                        {
                            int r = highlight.r * 255;
                            if (r > 255) r = 255;
                            if (r < 0) r = 0;

                            int g = highlight.g * 255;
                            if (g > 255) g = 255;
                            if (g < 0) g = 0;

                            int b = highlight.b * 255;
                            if (b > 255) b = 255;
                            if (b < 0) b = 0;

                            QColor background(r, g, b);
                            item->setData(Qt::BackgroundRole, background);
                        }

                        if (app_config_.highlighting.value_based_enabled && text_color.isValid())
                        {
                            item->setData(Qt::TextColorRole, text_color);

                            if (text_bold)
                            {
                                QFont font(item->font());
                                font.setBold(true);

                                item->setData(Qt::FontRole, font);
                            }
                        }
                    }
                }

            widget_->insertRow(widget_->rowCount());
        }
    }

}

void Table_View::rows_inserted(const QModelIndex&, int, int)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (app_config_.view_autoscroll)
        widget_->scrollToBottom();
}

void Table_View::on_autoscroll_change(int state)
{
    app_config_.view_autoscroll = (state == Qt::Checked ? true : false);
    emit autoscroll_change(state);
}

void Table_View::on_sorting_change(int state)
{
    app_config_.view_sorting = (state == Qt::Checked ? true : false);
    emit sorting_change(state);
}

void Table_View::clear_screen()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    trim_view(true);
}

void Table_View::on_clear_screen()
{
    emit clear_view();
}

void Table_View::reset_connected_state()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    QPushButton* stop_resume = widget_->parent()->findChild<QPushButton*>("connection_button");

    stop_resume->setText(tr("Start"));
    connected_ = false;
}

void Table_View::on_connection_stop_resume()
{
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);

        if (connected_)
        {
            QPushButton* stop_resume = widget_->parent()->findChild<QPushButton*>("connection_button");

            if (stop_resume)
                stop_resume->setText(tr("Resume"));

            connected_ = false;
        }
        else
        {
            QPushButton* stop_resume = widget_->parent()->findChild<QPushButton*>("connection_button");

            if (stop_resume)
                stop_resume->setText(tr("Stop"));

            connected_ = true;
        }
    }

    emit stop_resume();
}

void Table_View::on_quick_filter(const QString& text)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    quick_filter_ = text;
    apply_quick_filter();
}

void Table_View::apply_quick_filter()
{
    for (int i = 0; i < widget_->rowCount(); ++i)
    {
        bool match = false;

        if ((quick_filter_.length() >= 3) && (!quick_filter_.isEmpty()))
            for (int j = 0; j < widget_->columnCount(); ++j)
            {
                QTableWidgetItem *item = widget_->item(i, j);
                if (item && item->text().contains(quick_filter_))
                {
                    match = true;
                    break;
                }
            }
        else
            match = true;

        widget_->setRowHidden(i, !match);
    }
}

void Table_View::display_message(const QString &text)
{
    WindowWithMessageBoxInterface* wnd = dynamic_cast<WindowWithMessageBoxInterface*>(widget_->parent()->parent());
    if (!wnd) wnd = dynamic_cast<WindowWithMessageBoxInterface*>(widget_->parent());
    emit wnd->display_message(text);
}

void Table_View::display_details(QString details)
{
    QWidget *wnd = QApplication::activeWindow();
    Message_Details_Dialog* dlg = new Message_Details_Dialog(wnd, details);
    dlg->exec();
}

}}
