#include "app_settings_advanced.h"
#include "ui_app_settings_advanced.h"

App_Settings_Advanced::App_Settings_Advanced(fpgui::settings::App_Configuration &app_config, QWidget *parent):
QDialog(parent),
ui(new Ui::App_Settings_Advanced),
app_config_(app_config)
{
    ui->setupUi(this);
    ui->edit_batch_size->setValidator(new QIntValidator(1, 10000, this));
    ui->edit_clearing_ratio->setValidator(new QIntValidator(1, 99, this));
    ui->edit_view_refresh->setValidator(new QIntValidator(1, 10000, this));

    ui->edit_batch_size->setText(QString::number(app_config_.view_batch_size));
    ui->edit_clearing_ratio->setText(QString::number(app_config_.view_clearing_ratio));
    ui->edit_view_refresh->setText(QString::number(app_config_.view_refresh_time));
}

App_Settings_Advanced::~App_Settings_Advanced()
{
    delete ui;
}

void App_Settings_Advanced::on_App_Settings_Advanced_finished(int result)
{
    if (result == QDialog::Accepted)
    {
        app_config_.view_batch_size = ui->edit_batch_size->text().toInt();
        app_config_.view_clearing_ratio = ui->edit_clearing_ratio->text().toInt();
        app_config_.view_refresh_time = ui->edit_view_refresh->text().toInt();
    }
}
