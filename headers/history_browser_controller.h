#pragma once

#include <table_controller.h>
#include <history_browser_view.h>

class History_Browser_Controller: public fpgui::ui::Table_Controller
{
    Q_OBJECT

    public:

        History_Browser_Controller(fpgui::ui::History_Browser_View& view);


    public slots:

        void on_history_browse();
        void on_datetime_changed(qint64 start_datetime, qint64 end_datetime);

        void on_browse_back();
        void on_browse_forward();

        void on_clear_screen();

        void on_goto_page(int page);
        void on_per_page_changed(int per_page);

        void on_stop_resume();


    signals:

        void page_counter_update(int current_page, int total_pages);


    private:

        void request_data(std::queue<std::string>& data);
};
