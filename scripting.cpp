#include <memory>
#include <mutex>
#include <sstream>
#include <fstream>

#include <QtDebug>

#include <utils.h>
#include <scripting.h>
#include <lua/lua.hpp>

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

            std::string lua_error(get_lua_error());
            if (!lua_error.empty())
            {
                printf("lua_err on init = %s\n", lua_error.c_str());
            }

            inited_ = (lua_state_ != 0);
        }

        void deinit()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);

            if(lua_state_) lua_close(lua_state_);
            lua_state_ = 0;
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
