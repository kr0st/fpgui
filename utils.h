#pragma once

#include <string>
#include <vector>
#include <QString>

namespace generic_utils {

template <typename T> class Variable_Reset
{
    public:

        Variable_Reset(T& variable, T value_to_set, T value_to_reset):
        var_(variable),
        val_(value_to_reset)
        { var_ = value_to_set; }

        ~Variable_Reset(){ var_ = val_; }


    private:

        T& var_;
        T val_;
};

std::string get_username();
std::string& escape_quotes(std::string& str);
std::vector<std::string> strip_json(const std::string& fields_to_leave, const std::vector<std::string>& json_strings);

inline std::vector<std::string> tokenize(const char *str, char c = ' ')
{
    std::vector<std::string> result;

    do
    {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(std::string(begin, str));
    } while (0 != *str++);

    return result;
}

namespace date_time {

unsigned long long iso_timestamp_to_ms(const std::string& iso_8601_timestamp);

}

namespace crypto {

bool is_string_encrypted(std::string& str, unsigned char* key_64bit, int salt_size_bytes = 50);
std::string encrypt_string(std::string& cleartext, unsigned char* key_64bit, int salt_size_bytes = 50);
std::string decrypt_string(std::string& encrypted, unsigned char* key_64bit, int salt_size_bytes = 50, bool* error_condition = 0);

bool generate_encryption_key(unsigned char* generated_key_64bit);

}

namespace ui {

int message_box(const QString& msg);

}

}
