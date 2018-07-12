#include <globals.h>

namespace fpgui {
namespace settings {

const char* application_name = "fpgui";
const char* test_application_name = "fpgui_test";

const char* author = "kr0st";
const char* domain = "bioreactor.me";

const char* lua_file_name = "compare.lua";

const char* application_section_name = "application_settings";
const char* view_refresh_setting = "view_refresh_time";
const char* view_max_messages_setting = "view_max_visible_messages";
const char* view_batch_size_setting = "view_message_batch_size";
const char* view_clearing_ratio_setting = "view_clearing_ratio";
const char* app_window_width_setting = "window_width";
const char* app_window_height_setting = "window_height";
const char* view_autoscroll_setting = "view_autoscroll";
const char* view_sorting_setting = "view_sorting";

const char* db_section_name = "mongodb_settings";
const char* db_polling_interval_setting = "db_polling_interval_ms";
const char* db_host_setting = "db_host_address";
const char* db_collection_setting = "db_collection_name";
const char* db_port_setting = "db_port";
const char* db_user_setting = "db_user";
const char* db_password_setting = "db_user_password";
const char* db_auth_db_setting = "db_auth_db_name";

const char* tabs_section_name = "visible_tabs";
const char* tabs_array_name = "tabs";
const char* highlighting_array_name = "highlighting";
const char* tabs_sorting_setting = "sort_by";
const char* tabs_display_setting = "display";
const char* tabs_size_setting = "size";

const char* highlighting_base_color_setting = "highlighting_base_color";
const char* highlighting_diff_enabled_setting = "highlighting_differential_enabled";
const char* highlighting_value_based_enabled_setting = "highlighting_value_based_enabled";
const char* highlighting_field_setting = "hl_field";
const char* highlighting_value_setting = "hl_value";
const char* highlighting_color_setting = "hl_color";

const Tab_Names tab_names;
Tab_Names::Tab_Names():
name("name"),
facility("facility"), //according to syslog
priority("priority"), //same as severity for syslog
timestamp("timestamp"), //ISO8601 timestamp with milliseconds and timezone
hostname("hostname"), //IP address or any specific sending device id, added by fplogd before sending
appname("appname"), //name of application or service using this logging library, needed for fplog IPC
text("text"), //log message text
component("component"), //package name or any logical software component
class_name("class"), //class name if OOP is used
method("method"), //method of a given class if OOP is used or just a function name
module("module"), //source file name
line("line"), //line number in the above mentioned source file
options("options"), //for example encryption method + options when encryption is in use
encrypted("encrypted"), //true/false, if true then Text field contains encrypted JSON values -
                        //the rest of the log message including the plaintext version of Text field
file("file"), //filename when sending a file inside the log message
sequence("sequence") //sequential number of log message from a given hostname, helps to see which message
                     //was emitted earlier if several messages have identical timestamps
{
}

}
}
