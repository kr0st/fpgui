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

        Table_View(){}
        void setup_view(const std::vector<settings::Tab_Configuration> &config, QTableWidget& widget);
        std::vector<settings::Tab_Configuration> get_view_configuration();

        void close_view();


    signals:

        void closing();
};

}}

#endif // TABLE_VIEW_H
