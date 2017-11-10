#include "mainwindow.h"

#include <QApplication>
#include <QTableWidget>

#include <globals.h>
#include <settings.h>
#include <table_view.h>


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

    QWidget* central(w.centralWidget());


    //table.setup_view(fpgui::settings::read_tab_config(settings), *central);
    w.show();

    return a.exec();
}
