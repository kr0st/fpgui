#pragma once

#include <string>

namespace generic_utils {

std::string get_username();
std::string& escape_quotes(std::string& str);
std::vector<std::string> strip_json(const std::string& fields_to_leave, const std::vector<std::string>& json_strings);

namespace date_time {

unsigned long long iso_timestamp_to_ms(const std::string& iso_8601_timestamp);

}

namespace crypto {

bool is_string_encrypted(std::string& str, unsigned char* key_64bit, int salt_size_bytes = 50);
std::string encrypt_string(std::string& cleartext, unsigned char* key_64bit, int salt_size_bytes = 50);
std::string decrypt_string(std::string& encrypted, unsigned char* key_64bit, int salt_size_bytes = 50, bool* error_condition = 0);

bool generate_encryption_key(unsigned char* generated_key_64bit);

}
}
