#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <globals.h>

void default_settigs()
{
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
    settings.setValue(QString(fpgui::settings::tabs_section_name) + "/timestamp", "");

    return a.exec();
}
