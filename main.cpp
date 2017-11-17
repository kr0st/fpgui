#include "mainwindow.h"

#include <QApplication>
#include <QTableWidget>

#include <globals.h>
#include <settings.h>
#include <table_view.h>
#include <table_controller.h>
#include <utils.h>


class Closer: public QObject
{
    Q_OBJECT

    public:

        Closer();
        Closer(QApplication* app, fpgui::ui::Table_View* view): view_(view), app_(app) { connect(app, SIGNAL(lastWindowClosed()), this, SLOT(going_down())); }


    public slots:

        void going_down()
        {
            disconnect(app_, SIGNAL(lastWindowClosed()), this, SLOT(going_down()));
            view_->close_view();
        }


    private:

        fpgui::ui::Table_View* view_;
        QApplication* app_;
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName(fpgui::settings::author);
    QCoreApplication::setOrganizationDomain(fpgui::settings::domain);
    QCoreApplication::setApplicationName(fpgui::settings::application_name);

    fpgui::settings::make_config_path();

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    settings.setFallbacksEnabled(false);

    std::string section(fpgui::settings::db_section_name);
    section += '/';

    if (!settings.contains(QString((section + fpgui::settings::db_host_setting).c_str())))
    {
        settings.clear();
        fpgui::settings::write_default_settigs(settings);
    }

    fpgui::settings::create_default_script_file();

    MainWindow w;
    fpgui::ui::Table_View table;
    fpgui::ui::Table_Controller table_controller(table);

    QTableWidget* widget(w.centralWidget()->findChild<QTableWidget*>("tableWidget"));

    if (!widget)
    {
        generic_utils::ui::message_box("Fatal error: user interface is broken, exiting..");
        return -1;
    }

    w.show();
    table.setup_view(fpgui::settings::read_tab_config(settings), *widget);
    w.inject_table_view(&table);

    Closer closer(&a, &table);

    return a.exec();
}

#include "main.moc"
