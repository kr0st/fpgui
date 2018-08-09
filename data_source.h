#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <queue>
#include <string>
#include <QtCore>

namespace fpgui { namespace data_source { namespace random {

inline std::string random_timestamp()
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

inline void push_string(std::vector<std::string>& where, const std::string& what){ where.push_back(what); }
inline void push_string(std::queue<std::string>& where, const std::string& what){ where.push(what); }

template <typename T=std::vector<std::string>> void generate_json_strings(T& generated, size_t how_many, size_t text_size_bytes_min, size_t text_size_bytes_max)
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
        int text_size = static_cast<int>(text_size_bytes_min);

        if (text_size_bytes_max - text_size_bytes_min > 0)
            text_size += qrand() % static_cast<int>(text_size_bytes_max - text_size_bytes_min);

        std::string text;
        text.resize(static_cast<size_t>(text_size));

        for (int j = 0; j < text_size; ++j)
            text[static_cast<size_t>(j)] = random_bytes[static_cast<size_t>(j)];

        std::string rnd_msg = "{\"timestamp\":\"" + random_timestamp() + "\", \"sequence\":" + std::to_string(qrand() % 666) +
                              ", \"hostname\":\"" + hosts[static_cast<size_t>(qrand()) % hosts.size()] + "\", \"text\":\"" + text +  "\" }";
        push_string(generated, rnd_msg);
    }
}

}

template <typename T=std::queue<std::string>>
class Data_Source
{
  public:

      virtual void request_data(T& data) = 0;
      virtual int request_paged_data(unsigned, unsigned, T&){ return -1; }

      virtual void connect(QSettings&){}
      virtual void configure(std::map<QVariant, QVariant>&){}
      virtual void disconnect(){}

      virtual ~Data_Source(){}
};

template <typename T=std::queue<std::string>>
class Random_Data_Source: public Data_Source<T>
{
    public:

        void set_batch_size(int batch_sz){ set_batch_size(batch_sz, batch_sz); }
        void set_batch_size(int min_sz, int max_sz) { min_sz_batch_ = min_sz; max_sz_batch_ = max_sz; }

        void set_single_string_size(int string_sz) { set_single_string_size(string_sz, string_sz); }
        void set_single_string_size(int min_sz, int max_sz){ min_sz_string_ = min_sz; max_sz_string_ = max_sz; }

        void request_data(T &data)
        {
            int batch_sz = min_sz_batch_;

            if (max_sz_batch_ - min_sz_batch_ > 0)
                batch_sz += qrand() % (max_sz_batch_ - min_sz_batch_);

            random::generate_json_strings(data, batch_sz, min_sz_string_, max_sz_string_);
        }

        void connect(QSettings&){}


    private:

        int min_sz_batch_ = 0, max_sz_batch_ = 0, min_sz_string_ = 0, max_sz_string_ = 0;
};

}}

#endif // DATA_SOURCE_H
