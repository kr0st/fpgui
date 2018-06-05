#include "preferences.h"
#include "ui_preferences.h"

#include <QMessageBox>

#include <tabs_configuration.h>
#include <settings.h>
#include <utils.h>

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
    tab_config_ = dialog.get_configuration();
}

void Preferences::on_Preferences_finished(int result)
{
    if (result == QDialog::Accepted)
    {
        int question =
            generic_utils::ui::message_box("Application needs to quit in order to reset the settings.\n"
                                           "Pressing 'Cancel' will keep current settings.", QMessageBox::Ok | QMessageBox::Cancel);

        if (question == QMessageBox::Ok)
        {
            QSettings settings;
            QApplication::closeAllWindows();
            fpgui::settings::write_tab_config(tab_config_, settings);
        }
    }
}
