#ifndef HIGHLIGHTING_SETTINGS_H
#define HIGHLIGHTING_SETTINGS_H

#include <QDialog>
#include <QColor>

#include <settings.h>

namespace Ui {
class Highlighting_Settings;
}

class Highlighting_Settings : public QDialog
{
    Q_OBJECT

    public:

        explicit Highlighting_Settings(fpgui::settings::App_Configuration &app_config, QWidget *parent = nullptr);
        ~Highlighting_Settings();


    private slots:
        void on_Highlighting_Settings_finished(int result);


        void on_button_base_color_clicked();
        void on_button_add_clicked();
        void on_button_remove_clicked();

private:

            Ui::Highlighting_Settings *ui;
            fpgui::settings::App_Configuration& app_config_;
            QColor base_color_;
};

#endif // HIGHLIGHTING_SETTINGS_H
