#include <ios>
#include <iostream>

#include <QCoreApplication>
#include <QString>
#include <QTime>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include <globals.h>
#include <settings.h>
#include <mac_util.h>
#include <utils.h>
#include <simplecrypt.h>
#include <scripting.h>

#include <gtest/gtest.h>

#include <chrono>
#include <algorithm>
#include <date/date.h>

TEST(Util_Tests, Short_Size)
{
    EXPECT_EQ((size_t)2, sizeof(unsigned short));
}

TEST(Util_Tests, Mac_Address)
{
    unsigned char wrong_mac[6] = {6,6,6,9,9,9};
    unsigned char mac[6] = {6,6,6,9,9,9};

    EXPECT_GT(MACAddressUtility::GetMACAddress(mac), -1);
    EXPECT_NE(memcmp(wrong_mac, mac, 6), 0);
}

TEST(Encryption_Tests, Basic)
{
    unsigned char key[8] = {0};
    EXPECT_EQ(generic_utils::crypto::generate_encryption_key(key), true);
    SimpleCrypt crypto(*((quint64*)key));

    QString cypher = crypto.encryptToString(QString("plaintext"));
    EXPECT_NE(cypher.compare("plaintext"), 0);

    QString plaintext = crypto.decryptToString(cypher);
    EXPECT_EQ(plaintext.compare("plaintext"), 0);

    key[3] = 0;
    crypto.setKey(*((quint64*)key));
    plaintext = crypto.decryptToString(cypher);
    EXPECT_NE(plaintext.compare("plaintext"), 0);
}

TEST(Encryption_Tests, With_Salt)
{
    unsigned char key[8] = {0};
    EXPECT_EQ(generic_utils::crypto::generate_encryption_key(key), true);

    std::string cleartext("a Test password 123!@?");
    std::string cypher = generic_utils::crypto::encrypt_string(cleartext, key);

    std::string plaintext = generic_utils::crypto::decrypt_string(cypher, key);
    EXPECT_EQ(plaintext.compare(cleartext), 0);

    cleartext = "";
    cypher = generic_utils::crypto::encrypt_string(cleartext, key);

    plaintext = generic_utils::crypto::decrypt_string(cypher, key);
    EXPECT_EQ(plaintext.compare(cleartext), 0);

    cleartext = "U";
    cypher = generic_utils::crypto::encrypt_string(cleartext, key);

    plaintext = generic_utils::crypto::decrypt_string(cypher, key);
    EXPECT_EQ(plaintext.compare(cleartext), 0);

    cleartext = "ab";
    cypher = generic_utils::crypto::encrypt_string(cleartext, key);

    plaintext = generic_utils::crypto::decrypt_string(cypher, key);
    EXPECT_EQ(plaintext.compare(cleartext), 0);

    cleartext = "abc";
    cypher = generic_utils::crypto::encrypt_string(cleartext, key);

    plaintext = generic_utils::crypto::decrypt_string(cypher, key);
    EXPECT_EQ(plaintext.compare(cleartext), 0);
}

TEST(Encryption_Tests, With_No_Salt)
{
    unsigned char key[8] = {0};
    EXPECT_EQ(generic_utils::crypto::generate_encryption_key(key), true);

    std::string cleartext("a Test password 123!@?");
    std::string cypher = generic_utils::crypto::encrypt_string(cleartext, key, 0);
    EXPECT_EQ(cypher.length(), (size_t)0);

    std::string plaintext = generic_utils::crypto::decrypt_string(cypher, key, 0);
    EXPECT_EQ(cypher.length(), (size_t)0);
}

TEST(Encryption_Tests, With_Min_Salt)
{
    unsigned char key[8] = {0};
    EXPECT_EQ(generic_utils::crypto::generate_encryption_key(key), true);

    std::string cleartext("a Test password 123!@?");
    std::string cypher = generic_utils::crypto::encrypt_string(cleartext, key, 2);

    std::string plaintext = generic_utils::crypto::decrypt_string(cypher, key, 2);
    EXPECT_EQ(plaintext.compare(cleartext), 0);
}

TEST(Encryption_Tests, With_Salt_Smaller_Than_Input)
{
    unsigned char key[8] = {0};
    EXPECT_EQ(generic_utils::crypto::generate_encryption_key(key), true);

    std::string cleartext("a Test password 123!@?");
    std::string cypher = generic_utils::crypto::encrypt_string(cleartext, key, 10);

    std::string plaintext = generic_utils::crypto::decrypt_string(cypher, key, 10);
    EXPECT_EQ(plaintext.compare(cleartext), 0);
}

TEST(Encryption_Tests, With_Different_Salts)
{
    unsigned char key[8] = {0};
    EXPECT_EQ(generic_utils::crypto::generate_encryption_key(key), true);

    std::string cleartext("a Test password 123!@?");
    std::string cypher = generic_utils::crypto::encrypt_string(cleartext, key, 15);

    std::string plaintext = generic_utils::crypto::decrypt_string(cypher, key, 10);
    EXPECT_NE(plaintext.compare(cleartext), 0);
}

TEST(Encryption_Tests, Is_String_Encrypted)
{
    unsigned char key[8] = {0};
    EXPECT_EQ(generic_utils::crypto::generate_encryption_key(key), true);

    std::string cleartext("a Test password 123!@?");
    std::string cypher = generic_utils::crypto::encrypt_string(cleartext, key);

    EXPECT_EQ(generic_utils::crypto::is_string_encrypted(cypher, key), true);
    EXPECT_EQ(generic_utils::crypto::is_string_encrypted(cleartext, key), false);
    EXPECT_EQ(generic_utils::crypto::is_string_encrypted(cypher, key, 100), false);
    EXPECT_EQ(generic_utils::crypto::is_string_encrypted(cypher, key, 10), true);
    EXPECT_EQ(generic_utils::crypto::is_string_encrypted(cleartext, key, 10), false);

    QTime current_time(QTime::currentTime());
    qsrand(current_time.msecsSinceStartOfDay());

    int len_min = 5, len_max = 10000;
    for (int i = 0; i < 10000; ++i)
    {
        int str_len = len_min + qrand() % (len_max - len_min);
        int salt_len = qrand() % len_max;

        std::string str;
        str.resize(str_len);

        for (int j = 0; j < str_len; ++j)
            str[j] = (char)(32 + qrand() % (125 - 32));

        EXPECT_EQ(generic_utils::crypto::is_string_encrypted(str, key, salt_len), false);
    }
}

TEST(Util_Tests, Iso_Timestamp_Conversion)
{
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("2017-10-02T13:21:00.666+0200"), (unsigned long long)1506943260666);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("2017-10-02T13:21:00.668+0200"), (unsigned long long)1506943260668);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("2017-"), (unsigned)0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("+0300"), (unsigned)0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms(""), (unsigned)0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("+02"), (unsigned)0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("+987459276349872364i23gkjdshfgkjsdhgfkjsahvbfsdyif867wiuytg4234"), (unsigned)0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("2017-10-02T13:21:00.668+0000"), (unsigned long long)1506950460668);
}

void prepare_script_file()
{
    QFile script_file((fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name).c_str());
    script_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream toscriptfile(&script_file);

    toscriptfile << "if fplog_message1.hostname == fplog_message2.hostname then" << "\n";
    toscriptfile << " if fplog_message1.sequence > fplog_message2.sequence then" << "\n";
    toscriptfile << "  compare_result = 1" << "\n";
    toscriptfile << "  return compare_result" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.sequence < fplog_message2.sequence then" << "\n";
    toscriptfile << "  compare_result = -1" << "\n";
    toscriptfile << "  return compare_result" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";
    toscriptfile << "ts1 = convert_timestamp(fplog_message1.timestamp)" << "\n";
    toscriptfile << "ts2 = convert_timestamp(fplog_message2.timestamp)" << "\n";
    toscriptfile << "if ts1 > ts2 then" << "\n";
    toscriptfile << " compare_result = 1" << "\n";
    toscriptfile << " return compare_result" << "\n";
    toscriptfile << "end" << "\n";
    toscriptfile << "if ts1 < ts2 then" << "\n";
    toscriptfile << " compare_result = -1" << "\n";
    toscriptfile << " return compare_result" << "\n";
    toscriptfile << "end" << "\n";
    toscriptfile << "compare_result = 0" << "\n";
    toscriptfile << "return 0" << "\n";

    script_file.close();
}

TEST(Lua_Tests, Basic_Sorting)
{
    prepare_script_file();
    fpgui::lua::load_from_file(fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name);

    std::string cmp1, cmp2;
    cmp1 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\"}";
    cmp2 = "{\"timestamp\":\"2017-03-21T15:35:17.668+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.13\"}";

    EXPECT_EQ(fpgui::lua::compare_json_strings(cmp1, cmp2), -1);

    cmp1 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.10\" }";
    cmp2 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\" }";

    EXPECT_EQ(fpgui::lua::compare_json_strings(cmp1, cmp2), 1);

    cmp1 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.11\" }";
    cmp2 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\" }";

    EXPECT_EQ(fpgui::lua::compare_json_strings(cmp1, cmp2), 0);

    std::vector<std::string> correctly_sorted, contender;
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.11\" }");
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:17.876+0200\", \"sequence\": 5, \"hostname\":\"192.168.1.12\" }");
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:18.000+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\" }");
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:18.000+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.10\" }");
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:18.001+0200\", \"sequence\": 3, \"hostname\":\"192.168.1.11\" }");

    contender.push_back(correctly_sorted[4]);
    contender.push_back(correctly_sorted[3]);
    contender.push_back(correctly_sorted[1]);
    contender.push_back(correctly_sorted[0]);
    contender.push_back(correctly_sorted[2]);

    std::sort(contender.begin(), contender.end(), [](const std::string& s1, const std::string& s2) {
        int res = fpgui::lua::compare_json_strings(s1, s2);
        if ((res < -1) || (res == 0))
            return false;
        if (res == -1)
            return true;
        return false;
    });

    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(correctly_sorted[i], contender[i]);
    }
}

std::string random_timestamp()
{
    int y = 2000 + qrand() % 21;
    int m = 1 + qrand() % 11;
    int d = 1 + qrand() % 30;
    int h = qrand() % 23;
    int min = qrand() % 59;
    int s = qrand() % 59;
    int ms = qrand() % 999;
    int tz = qrand() % 6;

    char timestamp[256];
    snprintf(timestamp, sizeof(timestamp) - 1, "%04d-%02d-%02dT%02d:%02d:%02d.%03d+%02d00", y, m, d, h, min, s, ms, tz);
    return timestamp;
}

void generate_json_strings(std::vector<std::string>& generated, size_t how_many, size_t text_size_bytes_min, size_t text_size_bytes_max)
{
    if (text_size_bytes_max < text_size_bytes_min)
    {
        size_t temp = text_size_bytes_max;
        text_size_bytes_max = text_size_bytes_min;
        text_size_bytes_min = temp;
    }

    std::vector<std::string> hosts;

    hosts.push_back("192.168.3.8");
    hosts.push_back("192.168.4.6");
    hosts.push_back("192.168.1.12");
    hosts.push_back("192.168.1.10");
    hosts.push_back("192.168.1.3");
    hosts.push_back("192.168.5.10");
    hosts.push_back("192.168.4.41");
    hosts.push_back("192.168.1.114");
    hosts.push_back("192.168.2.11");
    hosts.push_back("192.168.1.12");

    std::string random_bytes;
    random_bytes.resize(text_size_bytes_max);

    for (size_t i = 0; i < text_size_bytes_max; ++i)
        random_bytes[i] = (65 + qrand() % 25);

    for (size_t i = 0; i < how_many; ++i)
    {
        int text_size = text_size_bytes_min + qrand() % (text_size_bytes_max - text_size_bytes_min);
        std::string text;
        text.resize(text_size);
        for (int j = 0; j < text_size; ++j)
            text[j] = random_bytes[j];
        std::string rnd_msg = "{\"timestamp\":\"" + random_timestamp() + "\", \"sequence\":" + std::to_string(qrand() % 666) +
                              ", \"hostname\":\"" + hosts[qrand() % hosts.size()] + "\", \"text\":\"" + text +  "\" }";
        generated.push_back(rnd_msg);
    }
}

TEST(Lua_Tests, Sorting_Performance)
{
    prepare_script_file();
    fpgui::lua::load_from_file(fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name);

    std::cout << "Generating strings.." << std::endl;

    std::vector<std::string>* strings = new std::vector<std::string>();
    size_t sz = 0;
    generate_json_strings(*strings, 10000, 0, 500);
    for (auto s: *strings)
        sz += s.size();
    sz /= strings->size();

    std::cout << "Average byte size of the test string: " << sz << std::endl;
    std::cout << "Stripping unnnecessary json.." << std::endl;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    *strings = generic_utils::strip_json("timestamp,hostname,sequence", *strings);

    std::cout << "Sort started.." << std::endl;

    std::sort(strings->begin(), strings->end(), [](const std::string& s1, const std::string& s2) {
        int res = fpgui::lua::compare_json_strings(s1, s2);
        if ((res < -1) || (res == 0))
            return false;
        if (res == -1)
            return true;
        return false;
    });

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sorted " << strings->size() << " strings using LuaScript in " << duration << " ms." << std::endl;
    std::cout << "Performance test ended." << std::endl;

    delete strings;
}

TEST(Util_Tests, Json_Stripper)
{
    std::vector<std::string> test, correct, contender;

    test.push_back("{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.11\" }");
    test.push_back("{\"timestamp\":\"2017-03-21T15:35:17.876+0200\", \"sequence\": 5, \"hostname\":\"192.168.1.12\" }");
    test.push_back("{\"timestamp\":\"2017-03-21T15:35:18.000+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\" }");
    test.push_back("{\"timestamp\":\"2017-03-21T15:35:18.000+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.10\" }");
    test.push_back("{\"timestamp\":\"2017-03-21T15:35:18.001+0200\", \"sequence\": 3, \"hostname\":\"192.168.1.11\" }");

    correct.push_back("{\"timestamp\":\"2017-03-21T15:35:17.666+0200\",\"hostname\":\"192.168.1.11\"}");
    correct.push_back("{\"timestamp\":\"2017-03-21T15:35:17.876+0200\",\"hostname\":\"192.168.1.12\"}");
    correct.push_back("{\"timestamp\":\"2017-03-21T15:35:18.000+0200\",\"hostname\":\"192.168.1.10\"}");
    correct.push_back("{\"timestamp\":\"2017-03-21T15:35:18.000+0200\",\"hostname\":\"192.168.1.10\"}");
    correct.push_back("{\"timestamp\":\"2017-03-21T15:35:18.001+0200\",\"hostname\":\"192.168.1.11\"}");

    contender = generic_utils::strip_json("timestamp,hostname", test);

    ASSERT_EQ(contender.size(), correct.size());
    for (size_t i = 0; i < correct.size(); ++i)
        EXPECT_EQ(contender[i], correct[i]);
}

void MessageHandler(QtMsgType, const QMessageLogContext & context, const QString & msg)
{
    static QMutex mutex;
    mutex.lock();

    QDateTime dateTime(QDateTime::currentDateTime());
    QString timeStr(dateTime.toString("dd-MM-yyyy HH:mm:ss:zzz"));

    QString contextString;
    if (context.file && (strlen(context.file) > 0) && (context.line > 0))
        contextString = QString("(%1, %2)").arg(context.file).arg(context.line);

    static QFile outFile("fpgui.log");
    if (!outFile.isOpen())
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream stream(&outFile);
    stream << timeStr << " " << contextString << ": " << msg << endl;
    std::cout << timeStr.toStdString() << " " << contextString.toStdString() << ": " << msg.toStdString() << std::endl;

    mutex.unlock();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(MessageHandler);

    QCoreApplication a(argc, argv);

    QCoreApplication::setOrganizationName(fpgui::settings::author);
    QCoreApplication::setOrganizationDomain(fpgui::settings::domain);
    QCoreApplication::setApplicationName(fpgui::settings::test_application_name);

    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;
    settings.setFallbacksEnabled(false);

    settings.clear();
    fpgui::settings::write_default_settigs(settings);

    {
        QSettings temp_settings;
        std::vector<fpgui::settings::Tab_Configuration> tabs = fpgui::settings::read_tab_config(temp_settings);
        tabs = tabs;
    }

    fpgui::settings::make_config_path();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    return 0;
}
