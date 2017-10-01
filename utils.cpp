#ifndef WIN32
#include <limits.h>
#include <unistd.h>
#endif

#include <utils.h>
#include <mac_util.h>
#include <simplecrypt.h>

#include <QString>
#include <QByteArray>

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

static QByteArray get_salt(unsigned char* key_64bit, int salt_size_bytes = 50)
{
    unsigned char seed = 0;
    for (int i = 0; i < 8; ++i)
        seed = (seed ^ key_64bit[i]);
    qsrand(seed);

    QByteArray salt;
    for (int i = 0; i < salt_size_bytes; ++i)
        salt.append(qrand());

    return salt;
}

bool is_string_encrypted(std::string& str, unsigned char* key_64bit, int salt_size_bytes)
{
    if (str.length() < (size_t)salt_size_bytes)
        return false;

    bool failed = true;
    decrypt_string(str, key_64bit, salt_size_bytes, &failed);
    return !failed;
}

std::string encrypt_string(std::string& cleartext, unsigned char* key_64bit, int salt_size_bytes)
{
    if (!key_64bit || (salt_size_bytes < 2))
        return "";

    QByteArray salty_pass = get_salt(key_64bit, salt_size_bytes);
    short clear_size = (short)cleartext.length();
    int salt_size = salty_pass.length();
    if (salt_size != salt_size_bytes)
        return "";

    salty_pass[0] = (salty_pass[0] ^ ((unsigned char*)&clear_size)[0]);
    salty_pass[1] = (salty_pass[1] ^ ((unsigned char*)&clear_size)[1]);

    if ((salt_size - 2) > clear_size)
    {
        for (int i = 0; i < clear_size; ++i)
            salty_pass[i + 2] = (salty_pass[i + 2] ^ cleartext[i]);
    }
    else
    {
        for (int i = 2; i < salt_size; ++i)
            salty_pass[i] = (salty_pass[i] ^ cleartext[i - 2]);

        for (int i = (salt_size - 2); i < clear_size; ++i)
            salty_pass.append(cleartext[i]);
    }

    SimpleCrypt crypto(*((quint64*)key_64bit));
    QString encrypted(crypto.encryptToString(salty_pass.toBase64()));

    return encrypted.toStdString();
}

std::string decrypt_string(std::string& encrypted, unsigned char* key_64bit, int salt_size_bytes, bool *error_condition)
{
    bool failed = true;
    if (error_condition)
        *error_condition = failed;

    if (!key_64bit || (salt_size_bytes < 2) || (encrypted.length() == 0))
        return "";

    QByteArray salt(get_salt(key_64bit, salt_size_bytes));
    if (salt.length() != salt_size_bytes)
        return "";

    SimpleCrypt crypto(*((quint64*)key_64bit));
    QString decrypted(crypto.decryptToString(QString(encrypted.c_str())));

    QByteArray cleartext(QByteArray::fromBase64(decrypted.toStdString().c_str()));
    if (cleartext.length() < salt_size_bytes)
        return "";

    short clear_size = 0;
    ((unsigned char*)&clear_size)[0] = (cleartext[0] ^ salt[0]);
    ((unsigned char*)&clear_size)[1] = (cleartext[1] ^ salt[1]);

    if (clear_size < 0)
        return "";

    std::string result;
    result.resize((size_t)clear_size);

    for (int i = 0; i < clear_size; ++i)
    {
        if ((i + 2) < salt_size_bytes)
            result[i] = (cleartext[i + 2] ^ salt[i + 2]);
        else
            result[i] = cleartext[i + 2];
    }

    failed = false;
    if (error_condition)
        *error_condition = failed;

    return result;
}

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
