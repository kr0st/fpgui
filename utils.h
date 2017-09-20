#pragma once

#include <string>

namespace generic_utils {

std::string get_username();

namespace crypto {
bool generate_encryption_key(unsigned char* generated_key_64bit);
}
}
