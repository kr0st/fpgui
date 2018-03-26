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


    signals:

        void refresh_view(std::vector<std::string> data_batch, bool full_refresh = false);


    public slots:

        void on_view_closing();


    private slots:

        void refresh_view_internal();
        void on_autoscroll_change(int state);
        void on_sorting_change(int state);
        void on_clear_screen();
        void on_connection_stop_resume();


    public:

        Table_Controller(Table_View& view);
        ~Table_Controller();

        void start_refreshing_view();
        void stop_refreshing_view();

        void set_data_source(std::shared_ptr<data_source::Data_Source<std::queue<std::string>>> data_source)
        {
            data_source_.reset();
            data_source_ = data_source;
        }

        void display_message(const QString &text){ view_.display_message(text); }


    private:

        std::recursive_mutex mutex_;

        settings::App_Configuration app_config_;
        std::vector<settings::Tab_Configuration> tab_config_;
        std::shared_ptr<data_source::Data_Source<std::queue<std::string>>> data_source_;

        class Timer_Thread;
        std::unique_ptr<Timer_Thread> timer_thread_;

        void merge_view_config(const Table_View::View_Configuration& config);


    protected:

        Table_View& view_;
        std::vector<std::string> data_, display_data_;
};

}}

#endif // TABLE_CONTROLLER_H
