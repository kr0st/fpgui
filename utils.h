#pragma once

#include <string>

namespace generic_utils {

std::string get_username();

namespace crypto {

bool is_string_encrypted(std::string& str, unsigned char* key_64bit, int salt_size_bytes = 50);
std::string encrypt_string(std::string& cleartext, unsigned char* key_64bit, int salt_size_bytes = 50);
std::string decrypt_string(std::string& encrypted, unsigned char* key_64bit, int salt_size_bytes = 50, bool* error_condition = 0);

bool generate_encryption_key(unsigned char* generated_key_64bit);

}
}
