#ifndef TABS_CONFIGURATION_H
#define TABS_CONFIGURATION_H

#include <QDialog>
#include <settings.h>
#include <vector>

namespace Ui {
class Tabs_Configuration;
}

namespace fpgui { namespace ui {

class Tabs_Configuration : public QDialog
{
    Q_OBJECT

    public:

        explicit Tabs_Configuration(QWidget *parent = nullptr);
        ~Tabs_Configuration();
        std::vector<fpgui::settings::Tab_Configuration> get_configuration();


    private slots:

        void on_button_add_clicked();


        void on_Tabs_Configuration_finished(int result);

        void on_button_remove_clicked();

private:

        Ui::Tabs_Configuration *ui;
        std::vector<fpgui::settings::Tab_Configuration> tab_config_;

        void populate_table_widget();
        std::map<int, fpgui::settings::Tab_Configuration> remap_tabs();
};

}};

#endif // TABS_CONFIGURATION_H
