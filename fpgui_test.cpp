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

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
