#include "db_settings_advanced.h"
#include "ui_db_settings_advanced.h"

DB_Settings_Advanced::DB_Settings_Advanced(fpgui::settings::Db_Configuration &db_config, QWidget *parent):
QDialog(parent),
ui(new Ui::db_settings_advanced),
db_config_(db_config)
{
    ui->setupUi(this);

    ui->edit_db_polling->setValidator(new QIntValidator(1, 10000, this));
    ui->edit_db_user_pass->setEchoMode(QLineEdit::Password);

    ui->edit_auth_db_name->setText(db_config.auth_db.c_str());
    ui->edit_db_polling->setText(QString::number(db_config.polling_interval));
    ui->edit_db_user->setText(db_config.username.c_str());
    ui->edit_db_user_pass->setText(db_config.password.c_str());
}

DB_Settings_Advanced::~DB_Settings_Advanced()
{
    delete ui;
}

void DB_Settings_Advanced::on_db_settings_advanced_finished(int result)
{
    if (result == QDialog::Accepted)
    {
        db_config_.auth_db = ui->edit_auth_db_name->text().toStdString();
        db_config_.polling_interval = ui->edit_db_polling->text().toInt();
        db_config_.username = ui->edit_db_user->text().toStdString();
        db_config_.password = ui->edit_db_user_pass->text().toStdString();
    }
}
