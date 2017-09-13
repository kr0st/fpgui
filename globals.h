#ifndef GLOBALS_H
#define GLOBALS_H

namespace fpgui {
namespace settings {

extern const char* application_name;
extern const char* test_application_name;

extern const char* author;
extern const char* domain;

extern const char* tabs_section_name;
extern const char* tabs_array_name;
extern const char* tabs_sorting_setting;
extern const char* tabs_display_setting;
extern const char* tabs_size_setting;


struct Tab_Names
{
    Tab_Names();

    const char* name;
    const char* facility; //according to syslog
    const char* priority; //same as severity for syslog
    const char* timestamp; //ISO8601 timestamp with milliseconds and timezone
    const char* hostname; //IP address or any specific sending device id, added by fplogd before sending
    const char* appname; //name of application or service using this logging library, needed for fplog IPC
    const char* text; //log message text
    const char* component; //package name or any logical software component
    const char* class_name; //class name if OOP is used
    const char* method; //method of a given class if OOP is used or just a function name
    const char* module; //source file name
    const char* line; //line number in the above mentioned source file
    const char* options; //for example encryption method + options when encryption is in use
    const char* encrypted; //true/false, if true then Text field contains encrypted JSON values -
                           //the rest of the log message including the plaintext version of Text field
    const char* file; //filename when sending a file inside the log message
    const char* sequence;
};

extern const Tab_Names tab_names;

}
}

#endif // GLOBALS_H
