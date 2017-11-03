#include <var_injector.h>
#include <settings.h>
#include <scripting.h>

namespace fpgui {
namespace lua {

void inject_tab_sorting_config()
{
    QSettings settings;
    std::vector<fpgui::settings::Tab_Configuration> tabs = fpgui::settings::read_tab_config(settings);
    for (auto& tab: tabs)
    {
        #ifdef _UNIT_TEST
            if (tab.name == "text")
                tab.sort_by = true;
        #endif

        inject_variable(("sort_by_" + tab.name).c_str(), tab.sort_by ? 1 : 0);
    }
}

}
}
