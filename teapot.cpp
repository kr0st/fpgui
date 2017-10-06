#include <memory>
#include <mutex>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <teapot.h>
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

namespace fpgui {
namespace chai {

class Chai_Impl
{
    public:

        Chai_Impl(const char* chai_script):
        chai_script_(chai_script)
        {
            init();
        }

        ~Chai_Impl()
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

            std::string std_format("log_msg1=\"%s\";\nfplog_message1 = from_json(log_msg1);\nlog_msg2=\"%s\";\nfplog_message2 = from_json(log_msg2);\n");

        retry_parsing:

            int chai_len = static_cast<int>(log_msg1_escaped.length() + log_msg2_escaped.length() + 256);

            char* chai_script = new char[chai_len];
            memset(chai_script, 0, chai_len);
            std::auto_ptr<char> chai_script_ptr(chai_script);
#ifndef _LINUX
            _snprintf(chai_script, chai_len - 1, std_format.c_str(), log_msg1_escaped.c_str(), log_msg2_escaped.c_str());
#else
            sprintf(chai_script, std_format.c_str(), log_msg1_escaped.c_str(), log_msg2_escaped.c_str());
#endif
            std::string full_script(chai_script + chai_script_);

            try
            {
                chai_->eval(full_script);
            }
            catch (chaiscript::exception::eval_error&)
            {
                std_format = "var log_msg1=\"%s\";\nvar fplog_message1 = from_json(log_msg1);\nvar log_msg2=\"%s\";\nvar fplog_message2 = from_json(log_msg2);\n";
                goto retry_parsing;
            }
            catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
                std::cout << full_script << std::endl;
            }

            return compare_result_;
        }


    private:

        int compare_result_;
        std::recursive_mutex mutex_;
        chaiscript::ChaiScript* chai_;

        void init()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);

            chai_ = new chaiscript::ChaiScript();
            chai_->add(chaiscript::var(std::ref(compare_result_)), "compare_result");
            chai_->add(chaiscript::fun(&generic_utils::date_time::iso_timestamp_to_ms), "iso_timestamp_to_ms");
        }

        void deinit()
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);

            delete chai_;
        }

        std::string chai_script_;
};

static std::recursive_mutex g_chai_mutex;
static std::auto_ptr<Chai_Impl> g_chai;

void load_from_file(const std::string& filename)
{
    std::lock_guard<std::recursive_mutex> lock(g_chai_mutex);

    std::ifstream infile(filename);
    std::string line, script;

    while (std::getline(infile, line))
        script += (line + "\n");

    g_chai.reset(new Chai_Impl(script.c_str()));
}

int compare_json_strings(const std::string& json_str1, const std::string& json_str2)
{
    if (!g_chai.get())
        return 0;

    return g_chai->compare(json_str1, json_str2);
}

}
}
