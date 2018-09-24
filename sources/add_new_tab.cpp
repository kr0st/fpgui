#include "add_new_tab.h"
#include "ui_add_new_tab.h"

namespace fpgui { namespace ui {

Add_New_Tab::Add_New_Tab(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Add_New_Tab)
{
    ui->setupUi(this);
}

Add_New_Tab::~Add_New_Tab()
{
    delete ui;
}

fpgui::settings::Tab_Configuration Add_New_Tab::get_new_tab()
{
    return new_tab_;
}

void Add_New_Tab::on_Add_New_Tab_finished(int result)
{
    if (result == QDialog::Accepted)
    {
        new_tab_.name = ui->edit_tab_name->text().toStdString();
        new_tab_.show = ui->check_show->isChecked();
        new_tab_.sort_by = ui->check_sort->isChecked();
        new_tab_.size = 0;
    }
}

}};
