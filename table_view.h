#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <settings.h>

#include <QTableWidget>
#include <mutex>

namespace fpgui {
namespace ui {

class Table_View: public QObject
{
    Q_OBJECT

    public:

        Table_View(settings::App_Configuration& app_config): app_config_(app_config) { widget_ = 0; }
        void setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget& widget, bool resize_only = false);
        std::vector<settings::Tab_Configuration> get_view_configuration();

        void close_view();
        void do_resize();
        void refresh_view(std::vector<std::string>& data_batch, bool full_refresh = false);


    public slots:

        void col_size_changed(int col, int old_sz, int new_sz);


    signals:

        void closing();


    private:

        std::vector<settings::Tab_Configuration> config_;
        settings::App_Configuration app_config_;

        QTableWidget* widget_;
        std::recursive_mutex mutex_;

        void display_strings(std::vector<std::string>& json_strings);


    protected:

        std::vector<std::string> data_;
};

}}

#endif // TABLE_VIEW_H
