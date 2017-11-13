#ifndef TABLE_CONTROLLER_H
#define TABLE_CONTROLLER_H

#include <table_view.h>

#include <QObject>

namespace fpgui {
namespace ui {

class Table_Controller: public QObject
{
    Q_OBJECT


    public slots:

        void on_view_closing();


    public:

        Table_Controller(Table_View& view): view_(view) {}


    private:

        Table_View& view_;
};

}}

#endif // TABLE_CONTROLLER_H
