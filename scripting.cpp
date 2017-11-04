#include <memory>
#include <mutex>
#include <sstream>
#include <fstream>

#include <QtDebug>

#include <utils.h>
#include <scripting.h>
#include <luajit-2.0/lua.hpp>

#define lua_isnumber(L,n)	(lua_type(L, (n)) == LUA_TNUMBER)

namespace fpgui {
namespace lua {

static int convert_timestamp(lua_State *L)
{
    unsigned long long timestamp = 0;
    std::string ts(lua_tostring(L, 1));
    lua_pop(L, 1);
    timestamp = generic_utils::date_time::iso_timestamp_to_ms(ts);
    lua_pushinteger(L, timestamp);
    return 1;
}

class Lua_Impl
{
    public:

        Lua_Impl(const char* lua_script):
        lua_script_(lua_script)
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

            const char* format = "log_msg1=\"%s\"\nfplog_message1 = json.parse(log_msg1)\nlog_msg2=\"%s\"\nfplog_message2 = json.parse(log_msg2)\n";
            int lua_len = static_cast<int>(log_msg1_escaped.length() + log_msg2_escaped.length() + 256);

            char* lua_script = new char[lua_len];
            memset(lua_script, 0, lua_len);
            std::unique_ptr<char[]> lua_script_ptr(lua_script);

#ifndef _LINUX
            _snprintf(lua_script, lua_len - 1, format, log_msg1_escaped.c_str(), log_msg2_escaped.c_str());
#else
            sprintf(lua_script, format, log_msg1_escaped.c_str(), log_msg2_escaped.c_str());
#endif
            std::string full_script(lua_script + lua_script_);
            luaL_dostring(lua_state_, full_script.c_str());

            std::string lua_error(get_lua_error());
            if (!lua_error.empty())
            {
                printf("lua_err = %s\n", lua_error.c_str());
                deinit();
                init();
            }
            else
                lua_pop(lua_state_, 1);

            lua_getglobal(lua_state_, "compare_result");
            if (lua_isnumber(lua_state_, -1))
                compare_result_ = lua_tonumber(lua_state_, -1);
            lua_pop(lua_state_, 1);

            return compare_result_;
        }

        void inject_variable(const char* name, double value)
        {
            if (!name)
                return;

            std::lock_guard<std::recursive_mutex> lock(mutex_);

            const char* format = "%s=%f\n";
            int lua_len = static_cast<int>(strlen(name) + 256);

            char* lua_script = new char[lua_len];
            memset(lua_script, 0, lua_len);
            std::unique_ptr<char[]> lua_script_ptr(lua_script);

#ifndef _LINUX
            _snprintf(lua_script, lua_len - 1, format, name, value;
#else
            sprintf(lua_script, format, name, value);
#endif
            luaL_dostring(lua_state_, lua_script);

            std::string lua_error(get_lua_error());
            if (!lua_error.empty())
            {
                printf("lua_err = %s\n", lua_error.c_str());
                deinit();
                init();
            }
        }


    private:

        bool inited_;
        lua_State *lua_state_;

        int compare_result_;
        std::recursive_mutex mutex_;

        std::string get_lua_error()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            if (lua_type(lua_state_, 1) == LUA_TSTRING)
            {
                const char* status = lua_tostring(lua_state_, -1);
                std::string err(status ? status : "");
                lua_pop(lua_state_, 1);
                return err;
            }

            return "";
        }

        void init()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);

            lua_state_ = luaL_newstate();
            luaL_openlibs(lua_state_);
            //luaL_dostring(lua_state_, "package.path = package.path .. ';/Library/Frameworks/fplog.framework/Versions/Current/?.lua'");
            luaL_dostring(lua_state_, "json = require(\"json\")\n");
            lua_register(lua_state_, "convert_timestamp", convert_timestamp);

            std::string lua_error(get_lua_error());
            if (!lua_error.empty())
            {
                printf("lua_err on init = %s\n", lua_error.c_str());
            }

            inited_ = (lua_state_ != 0);
        }

        void deinit()
        {
            if (lua_state_ == 0)
                return;

            std::lock_guard<std::recursive_mutex> lock(mutex_);

            if(lua_state_) lua_close(lua_state_);
            lua_state_ = 0;
        }

        std::string lua_script_;
};

static std::recursive_mutex g_lua_mutex;
static Lua_Impl* g_lua = 0;

void load_from_file(const std::string& filename)
{
    std::lock_guard<std::recursive_mutex> lock(g_lua_mutex);

    std::ifstream infile(filename);
    std::string line, script;

    while (std::getline(infile, line))
        script += (line + "\n");

    delete g_lua;
    g_lua = new Lua_Impl(script.c_str());
}

int compare_json_strings(const std::string& json_str1, const std::string& json_str2)
{
    std::lock_guard<std::recursive_mutex> lock(g_lua_mutex);
    if (g_lua == 0)
        return 0;

    return g_lua->compare(json_str1, json_str2);
}

void inject_variable(const char* name, double value)
{
    std::lock_guard<std::recursive_mutex> lock(g_lua_mutex);
    if (g_lua)
        g_lua->inject_variable(name, value);
}

void free_resources()
{
    std::lock_guard<std::recursive_mutex> lock(g_lua_mutex);
    delete g_lua;
    g_lua = 0;
}

}
}
