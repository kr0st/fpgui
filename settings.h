#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <vector>

namespace fpgui {
namespace settings {

struct Tab_Configuration
{
    Tab_Configuration(): size(0), sort_by(false), show(false){}

    std::string name;
    size_t size;
    bool sort_by;
    bool show;
};

std::vector<Tab_Configuration> read_tab_config(QSettings& settings);
void write_tab_config(const std::vector<Tab_Configuration>& tab_config, QSettings& settings);

void write_default_settigs(QSettings& settings);

}
}

#endif // SETTINGS_H
