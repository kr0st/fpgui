#include "tabs_configuration.h"
#include "ui_tabs_configuration.h"

#include <QSettings>
#include <QCheckBox>
#include <utils.h>

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
        ui->table_tabs->verticalHeaderItem(i)->setTextAlignment(Qt::AlignCenter);

        QCheckBox* checkbox = new QCheckBox(ui->table_tabs);
        checkbox->setChecked(tab.show);
        checkbox->setStyleSheet("margin:auto;");

        ui->table_tabs->setCellWidget(i, 0, checkbox);

        checkbox = new QCheckBox(ui->table_tabs);
        checkbox->setChecked(tab.sort_by);
        checkbox->setStyleSheet("margin:auto;");

        ui->table_tabs->setCellWidget(i, 1, checkbox);

        i++;
    }
}

std::map<int, fpgui::settings::Tab_Configuration> Tabs_Configuration::remap_tabs()
{
    std::map<int, fpgui::settings::Tab_Configuration> remapped;

    for (int i = 0; i < ui->table_tabs->rowCount(); ++i)
    {
        int index = ui->table_tabs->verticalHeader()->visualIndex(i);
        remapped[index] = tab_config_[i];
    }

    return remapped;
}

void Tabs_Configuration::on_button_add_clicked()
{
    std::map<int, fpgui::settings::Tab_Configuration> remapped(remap_tabs());
    QString res("");

    for (int i = 0; i < ui->table_tabs->rowCount(); ++i)
    {
        QString str(remapped[i].name.c_str());
        str += "\n";
        res += str;
    }

    generic_utils::ui::message_box(res);
}
