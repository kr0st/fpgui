#ifndef TABLE_CONTROLLER_H
#define TABLE_CONTROLLER_H

#include <mutex>
#include <memory>

#include <table_view.h>
#include <settings.h>
#include <data_source.h>

#include <QObject>

namespace fpgui {
namespace ui {

class Table_Controller: public QObject
{
    Q_OBJECT


    public slots:

        void on_view_closing();
        void refresh_view();


    public:

        Table_Controller(Table_View& view);
        void stop_refreshing_view();
        void set_data_source(std::shared_ptr<data_source::Data_Source<std::queue<std::string>>> data_source){ data_source_ = data_source; }


    private:

        Table_View& view_;
        bool view_is_refreshing_;
        std::recursive_mutex mutex_;

        settings::App_Configuration app_config_;
        std::vector<settings::Tab_Configuration> tab_config_;
        std::shared_ptr<data_source::Data_Source<std::queue<std::string>>> data_source_;
        std::vector<std::string> data_;
};

}}

#endif // TABLE_CONTROLLER_H
