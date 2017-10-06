#ifndef TEAPOT_H
#define TEAPOT_H

#include <string>

namespace fpgui {
namespace chai {

void load_from_file(const std::string& filename);
int compare_json_strings(const std::string& json_str1, const std::string& json_str2);

}
}

#endif // TEAPOT_H
