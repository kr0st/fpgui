#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <settings.h>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

    public:

        explicit Preferences(QWidget *parent = 0);
        ~Preferences();


    private slots:

        void on_button_config_tabs_clicked();
        void on_Preferences_finished(int result);


    private:

        Ui::Preferences *ui;
        std::vector<fpgui::settings::Tab_Configuration> tab_config_;
};

#endif // PREFERENCES_H
