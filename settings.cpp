#include <settings.h>
#include <globals.h>
#include <utils.h>

namespace fpgui {
namespace settings {


std::vector<Tab_Configuration> read_tab_config(QSettings& settings)
{
    int array_sz = settings.beginReadArray(QString(fpgui::settings::tabs_section_name) + fpgui::settings::tabs_array_name);
    std::vector<Tab_Configuration> tabs;

    try
    {
        for (int i = 0; i < array_sz; ++i)
        {
            settings.setArrayIndex(i);

            Tab_Configuration tab_config;

            tab_config.name = std::string(settings.value(QString(fpgui::settings::tab_names.name)).toString().toStdString());
            tab_config.show = settings.value(QString(fpgui::settings::tabs_display_setting)).toBool();
            tab_config.sort_by = settings.value(QString(fpgui::settings::tabs_sorting_setting)).toBool();
            tab_config.size = settings.value(QString(fpgui::settings::tabs_size_setting)).toInt();

            tabs.push_back(tab_config);
        }
    }
    catch(...)
    {
        settings.endArray();
        return tabs;
    }

    settings.endArray();
    return tabs;
}

void write_tab_config(const std::vector<Tab_Configuration>& tab_config, QSettings& settings)
{
    settings.beginWriteArray(QString(fpgui::settings::tabs_section_name) + fpgui::settings::tabs_array_name);

    try
    {
        for (size_t i = 0; i < tab_config.size(); ++i)
        {
            settings.setArrayIndex(i);
            settings.setValue(QString(fpgui::settings::tab_names.name), QString(tab_config[i].name.c_str()));
            settings.setValue(QString(fpgui::settings::tabs_display_setting), tab_config[i].show);
            settings.setValue(QString(fpgui::settings::tabs_sorting_setting), tab_config[i].sort_by);
            settings.setValue(QString(fpgui::settings::tabs_size_setting), qint64(tab_config[i].size));
        }
    }
    catch(...)
    {
        settings.endArray();
        return;
    }

    settings.endArray();
}

Db_Configuration read_db_config(QSettings& settings)
{
    Db_Configuration db_config;
    std::string section(fpgui::settings::db_section_name);
    section += '/';

    db_config.hostname = settings.value(QString((section + fpgui::settings::db_host_setting).c_str())).toString().toStdString();
    db_config.username = settings.value(QString((section + fpgui::settings::db_user_setting).c_str())).toString().toStdString();
    db_config.collection = settings.value(QString((section + fpgui::settings::db_collection_setting).c_str())).toString().toStdString();
    db_config.password = settings.value(QString((section + fpgui::settings::db_password_setting).c_str())).toString().toStdString();

    db_config.polling_interval = settings.value(QString((section + fpgui::settings::db_polling_interval_setting).c_str())).toInt();
    db_config.port = settings.value(QString((section + fpgui::settings::db_port_setting).c_str())).toInt();

    return db_config;
}

void write_db_config(Db_Configuration& db_config, QSettings& settings)
{
    std::string section(fpgui::settings::db_section_name);
    section += '/';

    settings.setValue(QString((section + fpgui::settings::db_host_setting).c_str()), db_config.hostname.c_str());
    settings.setValue(QString((section + fpgui::settings::db_user_setting).c_str()), db_config.username.c_str());
    settings.setValue(QString((section + fpgui::settings::db_collection_setting).c_str()), db_config.collection.c_str());

    unsigned char key[8] = {0};
    generic_utils::crypto::generate_encryption_key(key);

    if (!generic_utils::crypto::is_string_encrypted(db_config.password, key))
        db_config.password = generic_utils::crypto::encrypt_string(db_config.password, key);

    settings.setValue(QString((section + fpgui::settings::db_password_setting).c_str()), db_config.password.c_str());

    settings.setValue(QString((section + fpgui::settings::db_polling_interval_setting).c_str()), db_config.polling_interval);
    settings.setValue(QString((section + fpgui::settings::db_port_setting).c_str()), db_config.port);
}

void write_default_settigs(QSettings& settings)
{
    std::vector<Tab_Configuration> tabs;
    Tab_Configuration tab_config;

    tab_config.name = fpgui::settings::tab_names.timestamp;
    tab_config.show = true;
    tab_config.sort_by = true;
    tab_config.size = 0;
    tabs.push_back(tab_config);

    tab_config.name = fpgui::settings::tab_names.hostname;
    tab_config.show = true;
    tab_config.sort_by = true;
    tab_config.size = 0;
    tabs.push_back(tab_config);

    tab_config.name = fpgui::settings::tab_names.sequence;
    tab_config.show = false;
    tab_config.sort_by = true;
    tab_config.size = 0;
    tabs.push_back(tab_config);

    tab_config.name = fpgui::settings::tab_names.module;
    tab_config.show = true;
    tab_config.sort_by = false;
    tab_config.size = 0;
    tabs.push_back(tab_config);

    tab_config.name = fpgui::settings::tab_names.method;
    tab_config.show = true;
    tab_config.sort_by = false;
    tab_config.size = 0;
    tabs.push_back(tab_config);

    tab_config.name = fpgui::settings::tab_names.priority;
    tab_config.show = true;
    tab_config.sort_by = false;
    tab_config.size = 0;
    tabs.push_back(tab_config);

    tab_config.name = fpgui::settings::tab_names.text;
    tab_config.show = true;
    tab_config.sort_by = false;
    tab_config.size = 0;
    tabs.push_back(tab_config);

    Db_Configuration db_config;
    db_config.collection = "fplog.logs";
    db_config.hostname = "127.0.0.1";
    db_config.port = 27017;
    db_config.polling_interval = 1000;

    write_db_config(db_config, settings);
    write_tab_config(tabs, settings);
}

}
}
