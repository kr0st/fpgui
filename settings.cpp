#include <settings.h>
#include <globals.h>

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

    write_tab_config(tabs, settings);
}

}
}
