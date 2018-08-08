#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QTextStream>

#include <settings.h>
#include <globals.h>
#include <utils.h>

namespace fpgui {
namespace settings {

std::string get_config_path()
{
    char *home = getenv("HOME");
    QString path = home;

    path += "/.config/";
    path += QCoreApplication::organizationDomain();

    return path.toStdString();
}

void make_config_path()
{
    char *home = getenv("HOME");
    QString path = home;

    path +="/.config/";
    if (!QDir(path).exists())
        QDir().mkdir(path);

    path += QCoreApplication::organizationDomain();

    if (!QDir(path).exists())
        QDir().mkdir(path);
}

App_Configuration read_app_config(QSettings& settings)
{
    App_Configuration app_config;
    std::string section(fpgui::settings::application_section_name);
    section += '/';

    app_config.view_batch_size = settings.value(QString((section + fpgui::settings::view_batch_size_setting).c_str())).toInt();
    app_config.view_max_messages = settings.value(QString((section + fpgui::settings::view_max_messages_setting).c_str())).toInt();

    app_config.view_refresh_time = settings.value(QString((section + fpgui::settings::view_refresh_setting).c_str())).toInt();
    app_config.view_clearing_ratio = settings.value(QString((section + fpgui::settings::view_clearing_ratio_setting).c_str())).toInt();

    app_config.view_autoscroll = settings.value(QString((section + fpgui::settings::view_autoscroll_setting).c_str())).toBool();
    app_config.view_sorting = settings.value(QString((section + fpgui::settings::view_sorting_setting).c_str())).toBool();

    app_config.window_height = settings.value(QString((section + fpgui::settings::app_window_height_setting).c_str())).toInt();
    app_config.window_width = settings.value(QString((section + fpgui::settings::app_window_width_setting).c_str())).toInt();

    app_config.highlighting.diff_enabled = settings.value(QString((section + fpgui::settings::highlighting_diff_enabled_setting).c_str())).toBool();
    app_config.highlighting.value_based_enabled = settings.value(QString((section + fpgui::settings::highlighting_value_based_enabled_setting).c_str())).toBool();
    app_config.highlighting.base_color = settings.value(QString((section + fpgui::settings::highlighting_base_color_setting).c_str())).toString();

    int array_sz = settings.beginReadArray(QString(fpgui::settings::application_section_name) + "/" + fpgui::settings::highlighting_array_name);
    try
    {
        for (int i = 0; i < array_sz; ++i)
        {
            settings.setArrayIndex(i);

            Highlighting_Configuration::Config_Item item;

            item.field = std::string(settings.value(QString(fpgui::settings::highlighting_field_setting)).toString().toStdString());
            item.value = std::string(settings.value(QString(fpgui::settings::highlighting_value_setting)).toString().toStdString());
            item.color = settings.value(QString(fpgui::settings::highlighting_color_setting)).toString();
            item.bold = settings.value(QString(fpgui::settings::highlighting_bold_setting)).toBool();

            app_config.highlighting.config.push_back(item);
        }
    }
    catch(...)
    {
        settings.endArray();
        return app_config;
    }

    settings.endArray();
    return app_config;
}

void write_app_config(const App_Configuration& app_config, QSettings& settings)
{
    std::string section(fpgui::settings::application_section_name);
    section += '/';

    settings.setValue(QString((section + fpgui::settings::view_batch_size_setting).c_str()), app_config.view_batch_size);
    settings.setValue(QString((section + fpgui::settings::view_max_messages_setting).c_str()), app_config.view_max_messages);

    settings.setValue(QString((section + fpgui::settings::view_refresh_setting).c_str()), app_config.view_refresh_time);
    settings.setValue(QString((section + fpgui::settings::view_clearing_ratio_setting).c_str()), app_config.view_clearing_ratio);

    settings.setValue(QString((section + fpgui::settings::view_autoscroll_setting).c_str()), app_config.view_autoscroll);
    settings.setValue(QString((section + fpgui::settings::view_sorting_setting).c_str()), app_config.view_sorting);

    settings.setValue(QString((section + fpgui::settings::app_window_height_setting).c_str()), app_config.window_height);
    settings.setValue(QString((section + fpgui::settings::app_window_width_setting).c_str()), app_config.window_width);

    settings.setValue(QString((section + fpgui::settings::highlighting_diff_enabled_setting).c_str()), app_config.highlighting.diff_enabled);
    settings.setValue(QString((section + fpgui::settings::highlighting_value_based_enabled_setting).c_str()), app_config.highlighting.value_based_enabled);
    settings.setValue(QString((section + fpgui::settings::highlighting_base_color_setting).c_str()), app_config.highlighting.base_color.name());

    settings.beginWriteArray(QString(fpgui::settings::application_section_name) + "/" + fpgui::settings::highlighting_array_name);

    try
    {
        for (size_t i = 0; i < app_config.highlighting.config.size(); ++i)
        {
            settings.setArrayIndex(i);
            settings.setValue(QString(fpgui::settings::highlighting_field_setting), app_config.highlighting.config[i].field.c_str());
            settings.setValue(QString(fpgui::settings::highlighting_value_setting), app_config.highlighting.config[i].value.c_str());
            settings.setValue(QString(fpgui::settings::highlighting_color_setting), app_config.highlighting.config[i].color.name());
            settings.setValue(QString(fpgui::settings::highlighting_bold_setting), app_config.highlighting.config[i].bold);
        }
    }
    catch(...)
    {
        settings.endArray();
        return;
    }

    settings.endArray();
}

std::vector<Tab_Configuration> read_tab_config(QSettings& settings)
{
    int array_sz = settings.beginReadArray(QString(fpgui::settings::tabs_section_name) + "/" + fpgui::settings::tabs_array_name);
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
            tab_config.size = settings.value(QString(fpgui::settings::tabs_size_setting)).toDouble();

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
    settings.beginWriteArray(QString(fpgui::settings::tabs_section_name) + "/" + fpgui::settings::tabs_array_name);

    try
    {
        for (size_t i = 0; i < tab_config.size(); ++i)
        {
            settings.setArrayIndex(i);
            settings.setValue(QString(fpgui::settings::tab_names.name), QString(tab_config[i].name.c_str()));
            settings.setValue(QString(fpgui::settings::tabs_display_setting), tab_config[i].show);
            settings.setValue(QString(fpgui::settings::tabs_sorting_setting), tab_config[i].sort_by);
            settings.setValue(QString(fpgui::settings::tabs_size_setting), tab_config[i].size);
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
    db_config.auth_db = settings.value(QString((section + fpgui::settings::db_auth_db_setting).c_str())).toString().toStdString();

    db_config.polling_interval = settings.value(QString((section + fpgui::settings::db_polling_interval_setting).c_str())).toInt();
    db_config.port = settings.value(QString((section + fpgui::settings::db_port_setting).c_str())).toInt();

    unsigned char key[8] = {0};
    generic_utils::crypto::generate_encryption_key(key);

    if(!generic_utils::crypto::is_string_encrypted(db_config.password, key))
        write_db_config(db_config, settings);

    return db_config;
}

void write_db_config(Db_Configuration& db_config, QSettings& settings)
{
    std::string section(fpgui::settings::db_section_name);
    section += '/';

    settings.setValue(QString((section + fpgui::settings::db_host_setting).c_str()), db_config.hostname.c_str());
    settings.setValue(QString((section + fpgui::settings::db_user_setting).c_str()), db_config.username.c_str());
    settings.setValue(QString((section + fpgui::settings::db_collection_setting).c_str()), db_config.collection.c_str());
    settings.setValue(QString((section + fpgui::settings::db_auth_db_setting).c_str()), db_config.auth_db.c_str());

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
    db_config.auth_db = "admin";

    App_Configuration app_config;
    app_config.view_batch_size = 200;
    app_config.view_max_messages = 10000;
    app_config.view_refresh_time = 500;
    app_config.view_autoscroll = true;
    app_config.view_clearing_ratio = 2;
    app_config.view_sorting = true;

    app_config.highlighting.diff_enabled = true;
    app_config.highlighting.value_based_enabled = true;
    app_config.highlighting.base_color = QColor(0xd6, 0xf6, 0xdd);

    Highlighting_Configuration::Config_Item hl_item;
    hl_item.bold = true;
    hl_item.field = "priority";
    hl_item.value = "critical";
    hl_item.color = QColor(0xfc, 0x01, 0x07);
    app_config.highlighting.config.push_back(hl_item);

    hl_item.value = "emergency";
    app_config.highlighting.config.push_back(hl_item);

    hl_item.value = "alert";
    app_config.highlighting.config.push_back(hl_item);

    hl_item.value = "error";
    app_config.highlighting.config.push_back(hl_item);

    hl_item.color = QColor(0xe1, 0xa8, 0x08);
    hl_item.value = "warning";
    app_config.highlighting.config.push_back(hl_item);

    hl_item.value = "notice";
    hl_item.bold = false;
    app_config.highlighting.config.push_back(hl_item);

    write_app_config(app_config, settings);
    write_db_config(db_config, settings);
    write_tab_config(tabs, settings);
}

void create_default_script_file(bool overwrite)
{
    QFile script_file((fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name).c_str());

    if (script_file.exists())
        if (!overwrite)
            return;

    script_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream toscriptfile(&script_file);

    toscriptfile << "compare_result = 0" << "\n";
    toscriptfile << "local total1 = 0" << "\n";
    toscriptfile << "local total2 = 0" << "\n";

    toscriptfile << "if sort_by_text > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.text > fplog_message2.text then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_text)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.text < fplog_message2.text then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_text)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_hostname > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.hostname > fplog_message2.hostname then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_hostname)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.hostname < fplog_message2.hostname then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_hostname)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_sequence > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.sequence > fplog_message2.sequence then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_sequence)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.sequence < fplog_message2.sequence then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_sequence)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_module > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.module > fplog_message2.module then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_module)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.module < fplog_message2.module then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_module)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_method > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.method > fplog_message2.method then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_method)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.method < fplog_message2.method then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_method)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_priority > 0.1 then" << "\n";
    toscriptfile << " local prio = {}" << "\n";
    toscriptfile << " prio[\"debug\"]=0" << "\n";
    toscriptfile << " prio[\"info\"]=1" << "\n";
    toscriptfile << " prio[\"notice\"]=2" << "\n";
    toscriptfile << " prio[\"warning\"]=3" << "\n";
    toscriptfile << " prio[\"error\"]=4" << "\n";
    toscriptfile << " prio[\"critical\"]=5" << "\n";
    toscriptfile << " prio[\"alert\"]=6" << "\n";
    toscriptfile << " prio[\"emergency\"]=7" << "\n";
    toscriptfile << " if prio[fplog_message1.priority] > prio[fplog_message2.priority] then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_priority)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if prio[fplog_message1.priority] < prio[fplog_message2.priority] then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_priority)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_timestamp > 0.1 then" << "\n";
    toscriptfile << " local ts1 = convert_timestamp(fplog_message1.timestamp)" << "\n";
    toscriptfile << " local ts2 = convert_timestamp(fplog_message2.timestamp)" << "\n";
    toscriptfile << " if ts1 > ts2 then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_text)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if ts1 < ts2 then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_text)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if total1 > total2 then" << "\n";
    toscriptfile << " compare_result = 1" << "\n";
    toscriptfile << "end" << "\n";
    toscriptfile << "if total1 < total2 then" << "\n";
    toscriptfile << " compare_result = -1" << "\n";
    toscriptfile << "end" << "\n";
    toscriptfile << "return compare_result" << "\n";

    script_file.close();
}

}
}
