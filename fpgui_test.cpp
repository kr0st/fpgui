#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <globals.h>

void default_settigs(QSettings& settings)
{
    settings.beginWriteArray(QString(fpgui::settings::tabs_section_name) + fpgui::settings::tabs_array_name);

    settings.setArrayIndex(0);
    settings.setValue(QString(fpgui::settings::tab_names.name), QString(fpgui::settings::tab_names.timestamp));
    settings.setValue(QString(fpgui::settings::tabs_display_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_sorting_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_size_setting), 0);

    settings.setArrayIndex(1);
    settings.setValue(QString(fpgui::settings::tab_names.name), QString(fpgui::settings::tab_names.hostname));
    settings.setValue(QString(fpgui::settings::tabs_display_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_sorting_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_size_setting), 0);

    settings.setArrayIndex(2);
    settings.setValue(QString(fpgui::settings::tab_names.name), QString(fpgui::settings::tab_names.sequence));
    settings.setValue(QString(fpgui::settings::tabs_display_setting), false);
    settings.setValue(QString(fpgui::settings::tabs_sorting_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_size_setting), 0);

    settings.setArrayIndex(3);
    settings.setValue(QString(fpgui::settings::tab_names.name), QString(fpgui::settings::tab_names.module));
    settings.setValue(QString(fpgui::settings::tabs_display_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_sorting_setting), false);
    settings.setValue(QString(fpgui::settings::tabs_size_setting), 0);

    settings.setArrayIndex(4);
    settings.setValue(QString(fpgui::settings::tab_names.name), QString(fpgui::settings::tab_names.method));
    settings.setValue(QString(fpgui::settings::tabs_display_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_sorting_setting), false);
    settings.setValue(QString(fpgui::settings::tabs_size_setting), 0);

    settings.setArrayIndex(5);
    settings.setValue(QString(fpgui::settings::tab_names.name), QString(fpgui::settings::tab_names.priority));
    settings.setValue(QString(fpgui::settings::tabs_display_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_sorting_setting), false);
    settings.setValue(QString(fpgui::settings::tabs_size_setting), 0);

    settings.setArrayIndex(6);
    settings.setValue(QString(fpgui::settings::tab_names.name), QString(fpgui::settings::tab_names.text));
    settings.setValue(QString(fpgui::settings::tabs_display_setting), true);
    settings.setValue(QString(fpgui::settings::tabs_sorting_setting), false);
    settings.setValue(QString(fpgui::settings::tabs_size_setting), 0);

    settings.endArray();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setOrganizationName(fpgui::settings::author);
    QCoreApplication::setOrganizationDomain(fpgui::settings::domain);
    QCoreApplication::setApplicationName(fpgui::settings::test_application_name);

    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;
    settings.setFallbacksEnabled(false);

    settings.clear();
    default_settigs(settings);

    //return a.exec();
}
