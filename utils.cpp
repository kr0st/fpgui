#ifndef WIN32
#include <limits.h>
#include <unistd.h>
#endif

#include <utils.h>
#include <mac_util.h>

namespace generic_utils {

std::string get_username()
{
#ifdef WIN32
    #define INFO_BUFFER_SIZE 32767
    TCHAR  infoBuf[INFO_BUFFER_SIZE];
    DWORD  bufCharCount = INFO_BUFFER_SIZE;

    if (GetUserName(infoBuf, &bufCharCount))
        return infoBuf;
#else
    char username[1024];
    getlogin_r(username, 1024);
    return username;
#endif
}

namespace crypto {

bool generate_encryption_key(unsigned char* generated_key_64bit)
{
    if (!generated_key_64bit)
        return false;

    memset(generated_key_64bit, 0, 8);
    unsigned short padding = 0x029a;

    if (MACAddressUtility::GetMACAddress(generated_key_64bit) != 0)
        return false;

    std::string username(get_username());
    if (username.length() == 0)
        return false;

    unsigned char zero[8] = {0};
    if (memcmp(zero, generated_key_64bit, 8) == 0)
        return false;

    memcpy(&(generated_key_64bit[6]), &padding, sizeof(unsigned short));
    size_t len = 0;
    if (username.length() >= 8)
        len = 8;
    else
        len = username.length();

    for (size_t i = 0; i < len; ++i)
        generated_key_64bit[i] = (generated_key_64bit[i] ^ username.c_str()[i]);

    return true;
}

}
}
