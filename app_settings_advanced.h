#ifndef APP_SETTINGS_ADVANCED_H
#define APP_SETTINGS_ADVANCED_H

#include <settings.h>

#include <QDialog>

namespace Ui {
class App_Settings_Advanced;
}

class App_Settings_Advanced : public QDialog
{
    Q_OBJECT

public:

    explicit App_Settings_Advanced(fpgui::settings::App_Configuration& app_config, QWidget *parent = 0);
    ~App_Settings_Advanced();


private slots:
    void on_App_Settings_Advanced_finished(int result);

private:

    Ui::App_Settings_Advanced *ui;
    fpgui::settings::App_Configuration& app_config_;
};

#endif // APP_SETTINGS_ADVANCED_H
