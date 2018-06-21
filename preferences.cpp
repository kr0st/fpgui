#include "preferences.h"
#include "ui_preferences.h"

#include <QMessageBox>
#include <QProcess>

#include <tabs_configuration.h>
#include <app_settings_advanced.h>
#include <settings.h>
#include <utils.h>

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    ui->edit_max_visible->setValidator(new QIntValidator(1, 50000, this));

    QSettings settings;
    app_config_ = fpgui::settings::read_app_config(settings);
    db_config_ = fpgui::settings::read_db_config(settings);

    ui->edit_max_visible->setText(QString::number(app_config_.view_max_messages));
    ui->edit_collection->setText(db_config_.collection.c_str());
    ui->edit_host->setText(db_config_.hostname.c_str());
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::on_button_config_tabs_clicked()
{
    Tabs_Configuration dialog;
    dialog.exec();
    tab_config_ = dialog.get_configuration();
}

void Preferences::on_Preferences_finished(int result)
{
    app_config_.view_max_messages = ui->edit_max_visible->text().toInt();
    db_config_.collection = ui->edit_collection->text().toStdString();
    db_config_.hostname = ui->edit_host->text().toStdString();

    if (result == QDialog::Accepted)
    {
        int question =
            generic_utils::ui::message_box(tr("Application needs to restart in order to apply new settings.\n"
                                           "Pressing 'Cancel' will discard new settings."), QMessageBox::Ok | QMessageBox::Cancel);

        if (question == QMessageBox::Ok)
        {
            QSettings settings;

            QApplication::closeAllWindows();

            fpgui::settings::write_app_config(app_config_, settings);
            fpgui::settings::write_db_config(db_config_, settings);

            if ((tab_config_.size() > 0))
                fpgui::settings::write_tab_config(tab_config_, settings);

            QApplication::quit();
            QProcess::startDetached(QApplication::arguments()[0], QApplication::arguments());
        }
    }
}

void Preferences::on_button_advanced_view_clicked()
{
    App_Settings_Advanced dialog(app_config_);
    dialog.exec();
}
