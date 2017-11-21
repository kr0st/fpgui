#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <queue>
#include <string>

namespace fpgui { namespace data_source { namespace random {

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

}

class Data_Source
{
  public:

      virtual void request_data(std::queue<std::string>& data) = 0;
};

class Random_Data_Source: public Data_Source
{
  public:

      void request_data(std::queue<std::string> &data) {}
};

}}

#endif // DATA_SOURCE_H
