#include "tabs_configuration.h"
#include "ui_tabs_configuration.h"

Tabs_Configuration::Tabs_Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tabs_Configuration)
{
    ui->setupUi(this);
}

Tabs_Configuration::~Tabs_Configuration()
{
    delete ui;
}
