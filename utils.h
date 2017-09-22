#pragma once

#include <string>

namespace generic_utils {

std::string get_username();

namespace crypto {

bool is_password_encrypted(std::string& password, unsigned char* key_64bit);
std::string encrypt_password(std::string& cleartext_password, unsigned char* key_64bit);
std::string decrypt_password(std::string& encrypted_password, unsigned char* key_64bit);

bool generate_encryption_key(unsigned char* generated_key_64bit);

}
}
