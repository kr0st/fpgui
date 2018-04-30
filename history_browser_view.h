#pragma once

#include <QObject>
#include <table_controller.h>
#include <settings.h>

namespace fpgui { namespace ui {

class History_Browser_View : public Table_View
{
    Q_OBJECT


    public:

        History_Browser_View(settings::App_Configuration& app_config);

        void on_datetime_changed(qint64 start_datetime, qint64 end_datetime);
        int get_per_page_count();


    signals:

        void history_browse();
        void datetime_changed(qint64 start_datetime, qint64 end_datetime);

        void browse_back();
        void browse_forward();

        void per_page_changed(int per_page);
        void goto_page(int page);
        void item_activated(int index);


    public slots:

        void on_history_browse();
        void on_update_page_counter(int current_page, int total_pages);

        void on_browse_back() { emit browse_back(); }
        void on_browse_forward() { emit browse_forward(); }

        void on_per_page_changed(int per_page){ emit per_page_changed(per_page); }
        void on_goto_page(int page){ emit goto_page(page); }

        void on_item_activated(int index){ emit item_activated(index); }
};

}}
