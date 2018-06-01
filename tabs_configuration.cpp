#include "tabs_configuration.h"
#include "ui_tabs_configuration.h"

#include <QSettings>
#include <QCheckBox>

Tabs_Configuration::Tabs_Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tabs_Configuration)
{
    ui->setupUi(this);
    QSettings settings;
    tab_config_ = fpgui::settings::read_tab_config(settings);

    populate_table_widget();
}

Tabs_Configuration::~Tabs_Configuration()
{
    delete ui;
}

std::vector<fpgui::settings::Tab_Configuration> Tabs_Configuration::get_configuration()
{
    return tab_config_;
}

void Tabs_Configuration::populate_table_widget()
{
    ui->table_tabs->setRowCount(tab_config_.size());
    ui->table_tabs->setColumnCount(2);

    ui->table_tabs->verticalHeader()->setSectionsMovable(true);

    ui->table_tabs->setHorizontalHeaderItem(0, new QTableWidgetItem("Tab displayed"));
    ui->table_tabs->setHorizontalHeaderItem(1, new QTableWidgetItem("Used for sorting"));

    int i = 0;
    for (fpgui::settings::Tab_Configuration& tab : tab_config_)
    {
        ui->table_tabs->setVerticalHeaderItem(i, new QTableWidgetItem(tab.name.c_str()));

        QCheckBox* checkbox = new QCheckBox();
        checkbox->setChecked(tab.show);

        ui->table_tabs->setCellWidget(i, 0, checkbox);

        checkbox = new QCheckBox();
        checkbox->setChecked(tab.sort_by);

        ui->table_tabs->setCellWidget(i, 1, checkbox);

        i++;
    }
}
