#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <string>

namespace fpgui {
namespace lua {

void load_from_file(const std::string& filename);
int compare_json_strings(const std::string& json_str1, const std::string& json_str2);

}
}

#endif // SCRIPTING_H
