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

        Mongo_Data_Source(): client_(0) {}
        virtual ~Mongo_Data_Source() { delete client_; }

        void request_data(T& data);
        void connect(const settings::Db_Configuration& config);
        void connect(QSettings& settings);
        void disconnect();


    private:

        mongocxx::client* client_;
        std::string db_name_;
        std::string db_collection_name_;
};


}}

#endif // MONGO_DATA_SOURCE_H
