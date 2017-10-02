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

#include <gtest/gtest.h>

#include <chrono>
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
    for (int i = 0; i < 100000; ++i)
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
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("2017-10-02T13:21:00.666+0200"), 1506943260666);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("2017-10-02T13:21:00.668+0200"), 1506943260668);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("2017-"), 0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("+0300"), 0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms(""), 0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("+02"), 0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("+987459276349872364i23gkjdshfgkjsdhgfkjsahvbfsdyif867wiuytg4234"), 0);
    EXPECT_EQ(generic_utils::date_time::iso_timestamp_to_ms("2017-10-02T13:21:00.668+0200"), 1506943260668);
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

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
