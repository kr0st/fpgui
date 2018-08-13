#ifndef DB_SETTINGS_ADVANCED_H
#define DB_SETTINGS_ADVANCED_H

#include <QDialog>
#include <settings.h>

namespace Ui {
class db_settings_advanced;
}

class DB_Settings_Advanced : public QDialog
{
    Q_OBJECT

    public:

        explicit DB_Settings_Advanced(fpgui::settings::Db_Configuration& db_config, QWidget *parent = nullptr);
        ~DB_Settings_Advanced();


    private slots:

        void on_db_settings_advanced_finished(int result);


    private:

            Ui::db_settings_advanced *ui;
            fpgui::settings::Db_Configuration& db_config_;
};

#endif // DB_SETTINGS_ADVANCED_H
