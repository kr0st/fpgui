#ifndef ADD_NEW_TAB_H
#define ADD_NEW_TAB_H

#include <QDialog>
#include <settings.h>

namespace Ui {
class Add_New_Tab;
}

namespace fpgui { namespace ui {

class Add_New_Tab : public QDialog
{
    Q_OBJECT

    public:

        explicit Add_New_Tab(QWidget *parent = nullptr);
        ~Add_New_Tab();
        fpgui::settings::Tab_Configuration get_new_tab();


    private slots:

        void on_Add_New_Tab_finished(int result);


    private:

        Ui::Add_New_Tab *ui;
        fpgui::settings::Tab_Configuration new_tab_;
};

}};

#endif // ADD_NEW_TAB_H
