#ifndef VAR_INJECTOR_H
#define VAR_INJECTOR_H

#include<vector>

#include<settings.h>

namespace fpgui {
namespace lua {

void inject_tab_sorting_config();
void inject_tab_sorting_config(const std::vector<fpgui::settings::Tab_Configuration>& tabs);

}
}

#endif // VAR_INJECTOR_H
