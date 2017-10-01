#include <ios>
#include <iostream>

#include <QCoreApplication>
#include <QString>

#include <globals.h>
#include <settings.h>
#include <mac_util.h>
#include <utils.h>
#include <simplecrypt.h>

#include <gtest/gtest.h>

#include <chrono>
#include <date/date.h>

TEST(Util_Tests, Short_Size_Test)
{
    EXPECT_EQ(2, sizeof(unsigned short));
}

TEST(Util_Tests, Mac_Address_Test)
{
    unsigned char wrong_mac[6] = {6,6,6,9,9,9};
    unsigned char mac[6] = {6,6,6,9,9,9};

    EXPECT_GT(MACAddressUtility::GetMACAddress(mac), -1);
    EXPECT_NE(memcmp(wrong_mac, mac, 6), 0);
}

TEST(Util_Tests, Encryption_Test)
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

TEST(Util_Tests, Encryption_With_Salt_Test)
{
    unsigned char key[8] = {0};
    EXPECT_EQ(generic_utils::crypto::generate_encryption_key(key), true);

    std::string cleartext("a Test password 123!@?");
    std::string cypher = generic_utils::crypto::encrypt_string(cleartext, key);

    std::string plaintext = generic_utils::crypto::decrypt_string(cypher, key);
    EXPECT_EQ(plaintext.compare(cleartext), 0);
}

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

int main(int argc, char *argv[])
{
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

    auto tp(std::chrono::system_clock::now());
    std::string res;

    std::string datetime(date::format("%FT%T", tp));
    std::string tz_only(date::format("%z", tp));

    datetime += tz_only;
    for (auto it(datetime.begin()); it != datetime.end(); it++)
    {
        if ((*it == '.') || (*it == ','))
        {
            it += 4;

            res.append(datetime.begin(), it);
            res += "+0300";

            std::cout << res << std::endl;
            break;
        }
    }

    //days * 24 * 3600 + tp2.count() - offset.count() * 60
    auto days = days_from_civil(2017, 10, 1);

    std::chrono::minutes offset(0);
    std::chrono::seconds tp2(0);

    {
        std::stringstream ss(res);
        date::from_stream(ss, "%FT%T", tp2, (std::string*)(0), &offset);
    }

    std::string::iterator it(res.end());
    it -= 5;
    res.erase(res.begin(), it);

    {
        std::stringstream ss(res);
        date::from_stream(ss, "%z", tp2, (std::string*)(0), &offset);
    }

    std::cout << "date from stream:" << tp2.count() << ':' << offset.count() * 60 << std::endl;

    return 0;
    //::testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();
}
