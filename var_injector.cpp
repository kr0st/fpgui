#include <var_injector.h>
#include <settings.h>
#include <scripting.h>

namespace fpgui {
namespace lua {


void inject_tab_sorting_config()
{
    QSettings settings;
    std::vector<fpgui::settings::Tab_Configuration> tabs(fpgui::settings::read_tab_config(settings));
    inject_tab_sorting_config(tabs);
}

void inject_tab_sorting_config(const std::vector<fpgui::settings::Tab_Configuration>& tabs)
{
    int order = 1;
    for (const auto& tab: tabs)
    {
        #ifdef _UNIT_TEST
            if (tab.name == "text")
            {
                (const_cast<fpgui::settings::Tab_Configuration&>(tab)).sort_by = true;
                inject_variable(("sort_by_" + tab.name).c_str(), tab.sort_by ? 1 : 0);
                order++;
                continue;
            }
        #endif

        inject_variable(("sort_by_" + tab.name).c_str(), tab.sort_by ? order++ : 0);
    }
}

}
}
