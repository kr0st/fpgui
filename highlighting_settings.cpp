#include "highlighting_settings.h"
#include "ui_highlighting_settings.h"

Highlighting_Settings::Highlighting_Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Highlighting_Settings)
{
    ui->setupUi(this);
}

Highlighting_Settings::~Highlighting_Settings()
{
    delete ui;
}
