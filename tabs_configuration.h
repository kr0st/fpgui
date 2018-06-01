#ifndef TABS_CONFIGURATION_H
#define TABS_CONFIGURATION_H

#include <QDialog>
#include <settings.h>
#include <vector>

namespace Ui {
class Tabs_Configuration;
}

class Tabs_Configuration : public QDialog
{
    Q_OBJECT

    public:

        explicit Tabs_Configuration(QWidget *parent = 0);
        ~Tabs_Configuration();
        std::vector<fpgui::settings::Tab_Configuration> get_configuration();


    private:

        Ui::Tabs_Configuration *ui;
        std::vector<fpgui::settings::Tab_Configuration> tab_config_;

        void populate_table_widget();
};

#endif // TABS_CONFIGURATION_H
