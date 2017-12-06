#include <ios>
#include <iostream>
#include <thread>

#include <QCoreApplication>
#include <QString>
#include <QTime>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QThread>

#include <globals.h>
#include <settings.h>
#include <mac_util.h>
#include <utils.h>
#include <simplecrypt.h>
#include <scripting.h>
#include <var_injector.h>
#include <data_source.h>
#include <table_controller.h>
#include <table_view.h>

#include <gtest/gtest.h>

#include <chrono>
#include <algorithm>
#include <date/date.h>

class Test_View: public fpgui::ui::Table_View
{
    public:

        Test_View(fpgui::settings::App_Configuration& app_config): Table_View(app_config) {}
        std::vector<std::string> dump_display_data() { return this->data_; }
};

class Test_Controller: public fpgui::ui::Table_Controller
{
    public:

        Test_Controller(Test_View& view): Table_Controller(view) {}
        std::vector<std::string> dump_raw_data() { return this->data_; }
        std::vector<std::string> dump_display_data() { return this->display_data_; }
};

class Timer_Thread: public QThread
{
    public:

        Timer_Thread(Test_Controller& controller):
        controller_(controller)
        {
        }


    private:

        Test_Controller& controller_;

        void run()
        {
            controller_.refresh_view();
        }
};

TEST(Business_Logic, Table_Controller)
{
    QSettings settings;
    fpgui::lua::load_from_file(fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name);

    auto config(fpgui::settings::read_app_config(settings));
    Test_View view(config);
    Test_Controller controller(view);

    qsrand(13);

    auto source(std::make_shared<fpgui::data_source::Random_Data_Source<std::queue<std::string>>>());

    source->set_batch_size(config.view_batch_size / 2, config.view_batch_size * 2);
    source->set_single_string_size(10, 50);

    controller.set_data_source(source);

    Timer_Thread timer_thread(controller);
    timer_thread.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(12700));

    {
        std::vector<std::string> data(controller.dump_raw_data());
        for (auto& s : data)
            std::cout << s << std::endl;
        std::cout << std::endl << std::endl;
        std::vector<std::string> display_data(controller.dump_display_data());
        for (auto& s : display_data)
            std::cout << s << std::endl;
    }
}

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

void prepare_test_script_file(bool text_test = false)
{
    QFile script_file((fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name).c_str());
    script_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream toscriptfile(&script_file);

    toscriptfile << "compare_result = 0" << "\n";
    toscriptfile << "local total1 = 0" << "\n";
    toscriptfile << "local total2 = 0" << "\n";

    if (text_test)
    {
        toscriptfile << "if sort_by_text > 0.1 then" << "\n";
        toscriptfile << " if fplog_message1.text > fplog_message2.text then" << "\n";
        toscriptfile << "  compare_result = 1" << "\n";
        toscriptfile << " end" << "\n";
        toscriptfile << " if fplog_message1.text < fplog_message2.text then" << "\n";
        toscriptfile << "  compare_result = -1" << "\n";
        toscriptfile << " end" << "\n";
        toscriptfile << " return compare_result" << "\n";
        toscriptfile << "end" << "\n";
    }

    toscriptfile << "if sort_by_hostname > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.hostname > fplog_message2.hostname then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_hostname)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.hostname < fplog_message2.hostname then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_hostname)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_sequence > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.sequence > fplog_message2.sequence then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_sequence)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.sequence < fplog_message2.sequence then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_sequence)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_module > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.module > fplog_message2.module then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_module)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.module < fplog_message2.module then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_module)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_method > 0.1 then" << "\n";
    toscriptfile << " if fplog_message1.method > fplog_message2.method then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_method)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if fplog_message1.method < fplog_message2.method then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_method)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_priority > 0.1 then" << "\n";
    toscriptfile << " local prio = {}" << "\n";
    toscriptfile << " prio[\"debug\"]=0" << "\n";
    toscriptfile << " prio[\"info\"]=1" << "\n";
    toscriptfile << " prio[\"notice\"]=2" << "\n";
    toscriptfile << " prio[\"warning\"]=3" << "\n";
    toscriptfile << " prio[\"error\"]=4" << "\n";
    toscriptfile << " prio[\"critical\"]=5" << "\n";
    toscriptfile << " prio[\"alert\"]=6" << "\n";
    toscriptfile << " prio[\"emergency\"]=7" << "\n";
    toscriptfile << " if prio[fplog_message1.priority] > prio[fplog_message2.priority] then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_priority)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if prio[fplog_message1.priority] < prio[fplog_message2.priority] then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_priority)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if sort_by_timestamp > 0.1 then" << "\n";
    toscriptfile << " local ts1 = convert_timestamp(fplog_message1.timestamp)" << "\n";
    toscriptfile << " local ts2 = convert_timestamp(fplog_message2.timestamp)" << "\n";
    toscriptfile << " if ts1 > ts2 then" << "\n";
    toscriptfile << "  total1 = total1 + 10^(7-sort_by_text)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << " if ts1 < ts2 then" << "\n";
    toscriptfile << "  total2 = total2 + 10^(7-sort_by_text)" << "\n";
    toscriptfile << " end" << "\n";
    toscriptfile << "end" << "\n";

    toscriptfile << "if total1 > total2 then" << "\n";
    toscriptfile << " compare_result = 1" << "\n";
    toscriptfile << "end" << "\n";
    toscriptfile << "if total1 < total2 then" << "\n";
    toscriptfile << " compare_result = -1" << "\n";
    toscriptfile << "end" << "\n";
    toscriptfile << "return compare_result" << "\n";

    script_file.close();
}

TEST(Lua_Tests, Basic_Sorting)
{
    prepare_test_script_file();
    fpgui::lua::load_from_file(fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name);
    fpgui::lua::inject_tab_sorting_config();

    std::string cmp1, cmp2;
    cmp1 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\"}";
    cmp2 = "{\"timestamp\":\"2017-03-21T15:35:17.668+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.13\"}";

    EXPECT_EQ(fpgui::lua::compare_json_strings(cmp1, cmp2), -1);

    cmp1 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.10\" }";
    cmp2 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\" }";

    EXPECT_EQ(fpgui::lua::compare_json_strings(cmp1, cmp2), 1);

    cmp1 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.11\" }";
    cmp2 = "{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\" }";

    EXPECT_EQ(fpgui::lua::compare_json_strings(cmp1, cmp2), 1);

    std::vector<std::string> correctly_sorted, contender, contender2;
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:17.666+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.11\", \"text\":\"xyz\" }");
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:17.876+0200\", \"sequence\": 5, \"hostname\":\"192.168.1.12\", \"text\":\"bbkhh\" }");
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:18.000+0200\", \"sequence\": 1, \"hostname\":\"192.168.1.10\", \"text\":\"bchyg\" }");
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:18.000+0200\", \"sequence\": 2, \"hostname\":\"192.168.1.10\", \"text\":\"bchzg\" }");
    correctly_sorted.push_back("{\"timestamp\":\"2017-03-21T15:35:18.001+0200\", \"sequence\": 3, \"hostname\":\"192.168.1.11\", \"text\":\"abcj\" }");

    contender.push_back(correctly_sorted[4]);
    contender.push_back(correctly_sorted[3]);
    contender.push_back(correctly_sorted[1]);
    contender.push_back(correctly_sorted[0]);
    contender.push_back(correctly_sorted[2]);

    contender2 = contender;

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
        EXPECT_EQ(contender[i], correctly_sorted[i]);
    }

    prepare_test_script_file(true);
    fpgui::lua::load_from_file(fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name);
    fpgui::lua::inject_tab_sorting_config();

    std::sort(contender2.begin(), contender2.end(), [](const std::string& s1, const std::string& s2) {
        int res = fpgui::lua::compare_json_strings(s1, s2);
        if ((res < -1) || (res == 0))
            return false;
        if (res == -1)
            return true;
        return false;
    });

    auto temp = correctly_sorted[0];
    correctly_sorted[0] = correctly_sorted[correctly_sorted.size() - 1];
    correctly_sorted[correctly_sorted.size() - 1] = temp;

    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(contender2[i], correctly_sorted[i]);
    }
}

TEST(Lua_Tests, Sorting_Performance)
{
    prepare_test_script_file();
    fpgui::lua::load_from_file(fpgui::settings::get_config_path() + "/" + fpgui::settings::lua_file_name);
    fpgui::lua::inject_tab_sorting_config();

    std::cout << "Generating strings.." << std::endl;

    std::vector<std::string>* strings = new std::vector<std::string>();
    size_t sz = 0;
    fpgui::data_source::random::generate_json_strings(*strings, 1000, 0, 500);
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

    contender = generic_utils::strip_json("timestamp,hostname,", test);

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

std::recursive_mutex g_mutex;

void init(int argc, char *argv[])
{
    g_mutex.lock();
    QCoreApplication a(argc, argv);

    QCoreApplication::setOrganizationName(fpgui::settings::author);
    QCoreApplication::setOrganizationDomain(fpgui::settings::domain);
    QCoreApplication::setApplicationName(fpgui::settings::test_application_name);

    fpgui::settings::make_config_path();

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    settings.setFallbacksEnabled(false);
    settings.clear();

    fpgui::settings::write_default_settigs(settings);
    g_mutex.unlock();

    a.exec();
}

class Test_Thread: public QThread
{
    public:

        Test_Thread(int argc, char *argv[]):
        argc_(argc),
        argv_(argv)
        {
        }

        int get_exit_status(){ return exit_status_; }


    private:

        int argc_, exit_status_;
        char **argv_;

        void run()
        {
            g_mutex.lock();
            ::testing::InitGoogleTest(&argc_, argv_);
            exit_status_ = RUN_ALL_TESTS();
            g_mutex.unlock();
        }
};

int main(int argc, char *argv[])
{
    std::thread eventloop(init, argc, argv);
    Test_Thread test_thread(argc, argv);

    qInstallMessageHandler(MessageHandler);

    test_thread.start();
    test_thread.wait();
    int res = test_thread.get_exit_status();

    fpgui::lua::free_resources();
    exit(res);
}
