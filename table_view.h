#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <settings.h>

#include <QTableWidget>

namespace fpgui {
namespace ui {

class Table_View: public QObject
{
    Q_OBJECT

    public:

        Table_View(){ widget_ = 0; }
        void setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget& widget, bool resize_only = false);
        std::vector<settings::Tab_Configuration> get_view_configuration();

        void close_view();
        void do_delayed_resize();


    signals:

        void closing();


    private:

        std::vector<settings::Tab_Configuration> config_;
        QTableWidget* widget_;
};

}}

#endif // TABLE_VIEW_H
