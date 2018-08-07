#ifndef WIN32
#include <limits.h>
#include <unistd.h>
#endif

#include <algorithm>

#include <utils.h>
#include <simplecrypt.h>

#include <QNetworkInterface>
#include <QString>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QRegularExpression>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <date/date.h>

namespace generic_utils {

namespace ui {

int message_box(const QString &msg, int buttons)
{
    QMessageBox box((buttons == QMessageBox::Ok) ? QMessageBox::Icon::Warning : QMessageBox::Icon::Question,
                    "fpgui", msg, (QMessageBox::StandardButtons)buttons);

    QSpacerItem* vertical_spacer = new QSpacerItem(0, 50, QSizePolicy::Minimum);
    QGridLayout* layout = (QGridLayout*)box.layout();
    layout->addItem(vertical_spacer, 0, 1);
    return box.exec();
}

}

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

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

void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

QString to_alphanumeric(QString toClean)
{
    QString toReturn = toClean;
    toReturn.remove(QRegularExpression(QString::fromUtf8("[-`~!@#$%^&*()_—+=|:;<>«»,.?/{}\'\"\\[\\]\\\\]")));
    return toReturn;
}

std::string& escape_quotes(std::string& str)
{
    ReplaceStringInPlace(str, "\"", "\\\"");
    return str;
}

std::vector<std::string> strip_json(const std::string& fields_to_leave, const std::vector<std::string>& json_strings)
{
    std::vector<std::string> out_strings;
    std::vector<std::string> fields;

    std::istringstream is(fields_to_leave);
    std::string part;
    while (std::getline(is, part, ','))
      fields.push_back(part);

    for (const auto& js: json_strings)
    {
        rapidjson::GenericDocument<rapidjson::UTF8<>> js_from, js_to;
        std::unique_ptr<char[]> to_parse(new char[js.size() + 1]);

        memcpy(to_parse.get(), js.c_str(), js.size());
        to_parse.get()[js.size()] = 0;

        js_to.SetObject();
        js_from.ParseInsitu(to_parse.get());

        if (js_from.IsNull())
        {
            qCritical() << "JSON document is invalid!";
            continue;
        }

        bool file_found = false;

        if (js_from.IsObject())
        {
            rapidjson::Value::Object jsobj(js_from.GetObject());

            for (rapidjson::Value::Object::MemberIterator it = jsobj.MemberBegin(); it != jsobj.MemberEnd(); ++it)
            {
                std::string key(it->name.GetString());
                if (key.compare("file") == 0)
                    file_found = true;

                for (const auto& field: fields)
                    if (key.compare(field) == 0)
                    {
                        if (key.compare("text") == 0)
                        {
                            if (file_found)
                                js_to.AddMember(it->name, "this message contains attached file, see details (double click on this message)", js_to.GetAllocator());
                            else
                                js_to.AddMember(it->name, it->value, js_to.GetAllocator());
                        }
                        else
                            js_to.AddMember(it->name, it->value, js_to.GetAllocator());

                        break;
                    }
            }
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        js_to.Accept(writer);
        out_strings.push_back(buffer.GetString());
    }

    return out_strings;
}

bool validate_json(const std::string& mandatory_fields, const std::string &json_string)
{
    std::vector<std::string> fields;

    std::istringstream is(mandatory_fields);
    std::string part;
    while (std::getline(is, part, ','))
      fields.push_back(part);

    rapidjson::GenericDocument<rapidjson::UTF8<>> js_from;
    std::unique_ptr<char[]> to_parse(new char[json_string.size() + 1]);

    memcpy(to_parse.get(), json_string.c_str(), json_string.size());
    to_parse.get()[json_string.size()] = 0;

    js_from.ParseInsitu(to_parse.get());

    if (js_from.IsNull())
    {
        qCritical() << "JSON document validation failed!";
        return false;
    }

    if (js_from.IsObject())
    {
        rapidjson::Value::Object jsobj(js_from.GetObject());
        size_t total_fields = fields.size();
        size_t counted_fields = 0;

        for (rapidjson::Value::Object::MemberIterator it = jsobj.MemberBegin(); it != jsobj.MemberEnd(); ++it)
        {
            std::string key(it->name.GetString());
            for (const auto& field: fields)
                if (key.compare(field) == 0)
                {
                    counted_fields++;
                    break;
                }
        }

        if (total_fields != counted_fields)
            return false;
    }

    return true;
}

namespace date_time {

// Returns number of days since civil 1970-01-01.  Negative values indicate
//    days prior to 1970-01-01.
// Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
//                 m is in [1, 12]
//                 d is in [1, last_day_of_month(y, m)]
//                 y is "approximately" in
//                   [numeric_limits<Int>::min()/366, numeric_limits<Int>::max()/366]
//                 Exact range of validity is:
//                 [civil_from_days(numeric_limits<Int>::min()),
//                  civil_from_days(numeric_limits<Int>::max()-719468)]
template <class Int>
Int
days_from_civil(Int y, unsigned m, unsigned d) noexcept
{
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
             "This algorithm has not been ported to a 16 bit unsigned integer");
    static_assert(std::numeric_limits<Int>::digits >= 20,
             "This algorithm has not been ported to a 16 bit signed integer");
    y -= m <= 2;
    Int era = (y >= 0 ? y : y-399) / 400;
    unsigned yoe = static_cast<unsigned>(y - era * 400);      // [0, 399]
    unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
    unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
    return era * 146097 + static_cast<Int>(doe) - 719468;
}

unsigned long long iso_timestamp_to_ms(const std::string& iso_8601_timestamp)
{
    if (iso_8601_timestamp.empty())
        return 0;

    long long res = 0;
    std::string timestamp(iso_8601_timestamp);

    std::chrono::minutes tz_offset(0);
    std::chrono::milliseconds from_day_start(0);

    //getting milliseconds elapsed since the beginning of the day in the date,
    //real date and time zone are not taken into account for now
    try
    {
        std::stringstream ss(timestamp);
        date::from_stream(ss, "%FT%T", from_day_start, (std::string*)(0), &tz_offset);
    }
    catch (...)
    {
        return 0;
    }

    res += from_day_start.count();

    //leaving only time zone information in timestamp, getting time zone offset in minutes
    std::string::iterator it(timestamp.end());

    try
    {
        it -= 5;
        timestamp.erase(timestamp.begin(), it);

        std::stringstream ss(timestamp);
        date::from_stream(ss, "%z", from_day_start, (std::string*)(0), &tz_offset);

        res -= (tz_offset.count() * 60 * 1000);

        //finally getting date and turning date info into ms elapsed since January 1st, 1970
        timestamp = iso_8601_timestamp;
        it = timestamp.begin();
        it += 10;
        timestamp.erase(it, timestamp.end());
    }
    catch (...)
    {
        return 0;
    }

    if (timestamp.length() != 10)
        return 0;

    std::string year, month, day;
    year.resize(4);
    month.resize(2);
    day.resize(2);

    year[0] = timestamp[0];
    year[1] = timestamp[1];
    year[2] = timestamp[2];
    year[3] = timestamp[3];

    month[0] = timestamp[5];
    month[1] = timestamp[6];

    day[0] = timestamp[8];
    day[1] = timestamp[9];

    try
    {
        int y = std::stoi(year), m = std::stoi(month), d = std::stoi(day);
        if (y < 1970)
            return 0;
        if ((d <= 0) || (d > 31))
            return 0;
        if ((m <= 0) || (m > 12))
            return 0;

        long long days_from_epoch = days_from_civil(y, m, d);
        days_from_epoch *=  24 * 3600 * 1000;

        res += days_from_epoch;
    }
    catch (...)
    {
        return 0;
    }

    return (unsigned long long)res;
}

}

namespace crypto {

static QByteArray get_salt(unsigned char* key_64bit, int salt_size_bytes = 50)
{
    unsigned char seed = 0;
    for (int i = 0; i < 8; ++i)
        seed = (seed ^ key_64bit[i]);
    qsrand(seed);

    generic_utils::qStdOut() << "Seed=" << hex << seed << endl;

    QByteArray salt;
    for (int i = 0; i < salt_size_bytes; ++i)
        salt.append(qrand());

    generic_utils::qStdOut() << "Salt=" << hex << salt.toHex() << endl;
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

std::string decrypt_string(const std::string& encrypted, unsigned char* key_64bit, int salt_size_bytes, bool *error_condition)
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

#include <sys/utsname.h>
const char* getMachineName()
{
   static struct utsname u;

   if ( uname( &u ) < 0 )
      return "unknown";

   return u.nodename;
}

bool generate_encryption_key(unsigned char* generated_key_64bit)
{
    if (!generated_key_64bit)
        return false;

    memset(generated_key_64bit, 0, 8);

    const char* computername = getMachineName();
    unsigned char hash;
    unsigned int long_hash = 0;

    for (size_t i = 0; i < strlen(computername); ++i)
        long_hash += (unsigned int)computername[i];

    hash = long_hash % 255;

    generic_utils::qStdOut() << "hash=" << hash << ", long_hash=" << long_hash << endl;
    generic_utils::qStdOut() << "computername=" << computername << endl;

    std::string username(get_username());
    if (username.length() == 0)
        return false;

    memcpy(generated_key_64bit, computername, strlen(computername) > 8 ? 8 : strlen(computername));

    generic_utils::qStdOut() << "Username=" << username.c_str() << endl;

    unsigned char zero[8] = {0};
    if (memcmp(zero, generated_key_64bit, 8) == 0)
        return false;

    size_t len = 0;
    if (username.length() >= 8)
        len = 8;
    else
        len = username.length();

    for (size_t i = 0; i < len; ++i)
        generated_key_64bit[i] = (generated_key_64bit[i] ^ (unsigned char)username.c_str()[i] ^ hash);

    unsigned long long key = *(unsigned long long*)generated_key_64bit;
    generic_utils::qStdOut() << "generated_key_64bit=" << hex << key << endl;

    return true;
}

}
}
