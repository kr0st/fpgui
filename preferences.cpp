#include "preferences.h"
#include "ui_preferences.h"

#include <tabs_configuration.h>
#include <settings.h>

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
}

Preferences::~Preferences()
{
    delete ui;
}


void Preferences::on_button_config_tabs_clicked()
{
    Tabs_Configuration dialog;
    dialog.exec();
}
