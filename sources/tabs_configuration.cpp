#include "tabs_configuration.h"
#include "ui_tabs_configuration.h"

#include <QSettings>
#include <QCheckBox>
#include <utils.h>
#include <add_new_tab.h>

namespace fpgui { namespace ui {

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

void insert_row(QTableWidget* table_tabs, fpgui::settings::Tab_Configuration& tab)
{
    int i = table_tabs->rowCount();
    table_tabs->insertRow(i);

    table_tabs->setVerticalHeaderItem(i, new QTableWidgetItem(tab.name.c_str()));
    table_tabs->verticalHeaderItem(i)->setTextAlignment(Qt::AlignCenter);

    QCheckBox* checkbox = new QCheckBox(table_tabs);
    checkbox->setChecked(tab.show);
    checkbox->setStyleSheet("margin:auto;");

    table_tabs->setCellWidget(i, 0, checkbox);

    checkbox = new QCheckBox(table_tabs);
    checkbox->setChecked(tab.sort_by);
    checkbox->setStyleSheet("margin:auto;");

    table_tabs->setCellWidget(i, 1, checkbox);
}

void Tabs_Configuration::populate_table_widget()
{
    ui->table_tabs->setColumnCount(2);
    ui->table_tabs->verticalHeader()->setSectionsMovable(true);

    ui->table_tabs->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Tab displayed")));
    ui->table_tabs->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Used for sorting")));

    for (fpgui::settings::Tab_Configuration& tab : tab_config_)
        insert_row(ui->table_tabs, tab);
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
    bool complete = false;
    while (!complete)
    {
        Add_New_Tab add_new;
        int res = add_new.exec();
        if (res == 0)
        {
            complete = true;
            continue;
        }

        fpgui::settings::Tab_Configuration new_tab(add_new.get_new_tab());

        QString tab_name(new_tab.name.c_str());
        tab_name = tab_name.trimmed();
        tab_name = generic_utils::to_alphanumeric(tab_name);

        if (tab_name.size() < 3)
        {
            generic_utils::ui::message_box(tr("New tab name '") + QString(new_tab.name.c_str()) + tr("' cannot be shorter than 3 characters and cannot contain special symbols, including spaces."));
            continue;
        }

        auto tab = tab_config_.begin();
        for (; tab != tab_config_.end(); ++tab)
            if (tab->name.compare(tab_name.toStdString()) == 0)
            {
                generic_utils::ui::message_box(tr("New tab name '") + QString(new_tab.name.c_str()) + tr("' cannot be a duplicate of an existing tab."));
                break;
            }

        if (tab != tab_config_.end())
            continue;

        complete = true;
        new_tab.name = tab_name.toStdString();

        tab_config_.push_back(new_tab);
        insert_row(ui->table_tabs, new_tab);
    }
}

void Tabs_Configuration::on_Tabs_Configuration_finished(int result)
{
    if (result == QDialog::Accepted)
    {
        std::vector<fpgui::settings::Tab_Configuration> remapped;

        auto map(remap_tabs());
        for (size_t i = 0; i < map.size(); ++i)
            remapped.push_back(map[i]);

        tab_config_ = remapped;
    }
}

void Tabs_Configuration::on_button_remove_clicked()
{
    QItemSelectionModel *selectionModel = ui->table_tabs->selectionModel();

    if (selectionModel->selectedRows().size())
    {
        int row(selectionModel->selectedRows()[0].row());
        for (auto tab = tab_config_.begin(); tab != tab_config_.end(); ++tab)
            if (tab->name.compare(ui->table_tabs->verticalHeaderItem(row)->text().toStdString()) == 0)
            {
                ui->table_tabs->removeRow(row);
                tab_config_.erase(tab);
                break;
            }
    }
}

}};
