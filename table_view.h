#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <settings.h>

#include <QTableWidget>
#include <mutex>

class WindowWithMessageBoxInterface;

namespace fpgui {
namespace ui {

class Table_View: public QObject
{
    Q_OBJECT

    public:

        struct View_Configuration
        {
            std::vector<settings::Tab_Configuration> tab_config;
            settings::App_Configuration app_config;
        };

        Table_View(settings::App_Configuration& app_config): app_config_(app_config), connected_(false), quick_filter_("")
        {
            widget_ = 0;
        }
        void setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget& widget,
                        bool resize_only = false, WindowWithMessageBoxInterface* window = 0);
        View_Configuration get_view_configuration();

        void close_view();
        void do_resize();

        void on_autoscroll_change(int state);
        void on_sorting_change(int state);

        void on_clear_screen();
        void clear_screen();
        void on_connection_stop_resume();

        void on_quick_filter(const QString& text);
        void reset_connected_state();

        void display_message(const QString& text);
        void show_hide();


    public slots:

        void col_size_changed(int col, int old_sz, int new_sz);
        void rows_inserted(const QModelIndex & parent, int, int);
        void refresh_view(std::vector<std::string> data_batch, bool full_refresh = false);


    signals:

        void closing();
        void autoscroll_change(int state);
        void sorting_change(int state);
        void clear_view();
        void stop_resume();


    private:

        std::vector<settings::Tab_Configuration> config_;
        settings::App_Configuration app_config_;

        QTableWidget* widget_;
        std::recursive_mutex mutex_;
        bool connected_;
        QString quick_filter_;

        WindowWithMessageBoxInterface* window_;

        void display_strings(std::vector<std::string>& json_strings);
        void trim_view(bool clear_screen = false);
        void apply_quick_filter();


    protected:

        std::vector<std::string> data_;
};

}}

#endif // TABLE_VIEW_H
