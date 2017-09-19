#include <ios>
#include <iostream>

#include <QCoreApplication>
#include <QString>

#include <globals.h>
#include <settings.h>

#include <mac_util.h>

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

    {
        QSettings temp_settings;
        std::vector<fpgui::settings::Tab_Configuration> tabs = fpgui::settings::read_tab_config(temp_settings);
        tabs = tabs;
    }

    unsigned char mac[6] = {6,6,6,9,9,9};

    if (MACAddressUtility::GetMACAddress(mac) > -1)
    {
        std::cout << std::hex << (unsigned int)mac[0] << "-" << std::hex << (unsigned int)mac[1] << "-" << std::hex << (unsigned int)mac[2]
                  << "-" << std::hex << (unsigned int)mac[3] << "-" << std::hex << (unsigned int)mac[4] << "-" << std::hex << (unsigned int)mac[5]
                  << std::endl;
    }

    //return a.exec();
}
