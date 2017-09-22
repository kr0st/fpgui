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

struct Db_Configuration
{
    Db_Configuration(): port(0), polling_interval(0) {}

    std::string username;
    std::string hostname;
    std::string collection;
    std::string password;

    int port;
    int polling_interval;
};

std::vector<Tab_Configuration> read_tab_config(QSettings& settings);
void write_tab_config(const std::vector<Tab_Configuration>& tab_config, QSettings& settings);

Db_Configuration read_db_config(QSettings& settings);
void write_db_config(Db_Configuration& db_config, QSettings& settings);

void write_default_settigs(QSettings& settings);

}
}

#endif // SETTINGS_H
