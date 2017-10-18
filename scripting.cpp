#include <memory>
#include <mutex>
#include <sstream>
#include <fstream>

#include <QtDebug>

#include <utils.h>
#include <scripting.h>

namespace fpgui {
namespace lua {

class Lua_Impl
{
    public:

        Lua_Impl(const char* lua_script)
        {
            init();
        }

        ~Lua_Impl()
        {
            deinit();
        }

        int compare(const std::string& msg1, const std::string& msg2)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            compare_result_ = -3;

            std::string log_msg1_escaped(msg1);
            std::string log_msg2_escaped(msg2);

            generic_utils::escape_quotes(log_msg1_escaped);
            generic_utils::escape_quotes(log_msg2_escaped);

            return compare_result_;
        }


    private:

        int compare_result_;
        std::recursive_mutex mutex_;

        void init()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
        }

        void deinit()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
        }

        std::string lua_script_;
};

static std::recursive_mutex g_lua_mutex;
static std::auto_ptr<Lua_Impl> g_lua;

void load_from_file(const std::string& filename)
{
    std::lock_guard<std::recursive_mutex> lock(g_lua_mutex);

    std::ifstream infile(filename);
    std::string line, script;

    while (std::getline(infile, line))
        script += (line + "\n");

    g_lua.reset(new Lua_Impl(script.c_str()));
}

int compare_json_strings(const std::string& json_str1, const std::string& json_str2)
{
    if (!g_lua.get())
        return 0;

    return g_lua->compare(json_str1, json_str2);
}

}
}
