#include <QCoreApplication>
#include <QString>

#include <globals.h>
#include <settings.h>

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
    fpgui::settings::write_default_settigs(settings);

    //return a.exec();
}
