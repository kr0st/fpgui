#ifndef HIGHLIGHTING_SETTINGS_H
#define HIGHLIGHTING_SETTINGS_H

#include <QDialog>
#include <QColor>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>

#include <settings.h>

namespace Ui {
class Highlighting_Settings;
}

class Controls_Quadruple: QObject
{
    Q_OBJECT

    public:

        QLineEdit* name_edit;
        QLineEdit* value_edit;
        QPushButton* color_button;
        QCheckBox* bold_check;

        Controls_Quadruple():
        name_edit(nullptr), value_edit(nullptr), color_button(nullptr), bold_check(nullptr), color(Qt::black) {}
        virtual ~Controls_Quadruple(){}

        void connect_signals();

        QColor color;


    public slots:

        void on_color_button_clicked();
        void on_bold_check_state_changed(int state);
};

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
