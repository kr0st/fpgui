#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <vector>

namespace fpgui {
namespace settings {

struct App_Configuration
{
    App_Configuration():
    view_batch_size(0),
    view_max_messages(0),
    view_refresh_time(0),
    view_clearing_ratio(0),
    view_autoscroll(false),
    window_height(0),
    window_width(0)
    {}

    int view_batch_size;
    int view_max_messages;

    int view_refresh_time;
    int view_clearing_ratio;

    bool view_autoscroll;

    int window_height;
    int window_width;
};

struct Tab_Configuration
{
    Tab_Configuration(): size(0), sort_by(false), show(false){}

    std::string name;
    double size;
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

std::string get_config_path();
void make_config_path();

App_Configuration read_app_config(QSettings& settings);
void write_app_config(const App_Configuration& app_config, QSettings& settings);

std::vector<Tab_Configuration> read_tab_config(QSettings& settings);
void write_tab_config(const std::vector<Tab_Configuration>& tab_config, QSettings& settings);

Db_Configuration read_db_config(QSettings& settings);
void write_db_config(Db_Configuration& db_config, QSettings& settings);

void write_default_settigs(QSettings& settings);
void create_default_script_file(bool overwrite = false);

}
}

#endif // SETTINGS_H
