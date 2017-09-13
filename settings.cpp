#include <settings.h>
#include <globals.h>

namespace fpgui {
namespace settings {


std::vector<Tab_Configuration> read_tab_config(QSettings& settings)
{
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

}
}
