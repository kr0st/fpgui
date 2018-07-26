#ifndef MONGO_DATA_SOURCE_H
#define MONGO_DATA_SOURCE_H

#include <data_source.h>
#include <settings.h>
#include <QSettings>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <fpgui_exceptions.h>

namespace fpgui { namespace data_source {

template <typename T=std::queue<std::string>>
class Mongo_Data_Source: public Data_Source<T>
{
    public:

        Mongo_Data_Source(): client_(nullptr), time_start_(0), time_end_(0x7fffffff) {}
        virtual ~Mongo_Data_Source() { delete client_; }

        void request_data(T& data);
        int request_paged_data(unsigned page_number, unsigned per_page_count, T& data);

        void connect(const settings::Db_Configuration& config);
        void connect(QSettings& settings);
        void disconnect();
        void configure(std::map<QVariant, QVariant>& options);

        void set_timestamp_constraints(long long from, long long to){ time_start_ = from; time_end_ = to; }


    private:

        mongocxx::client* client_;
        std::string db_name_;
        std::string db_collection_name_;

        std::string first_id_;
        std::string last_id_;

        long long time_start_, time_end_;
};


}}

#endif // MONGO_DATA_SOURCE_H
