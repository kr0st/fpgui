#ifndef MONGO_DATA_SOURCE_H
#define MONGO_DATA_SOURCE_H

#include <data_source.h>
#include <settings.h>
#include <QSettings>
#include <mongocxx/client.hpp>

namespace fpgui { namespace data_source {

template <typename T=std::queue<std::string>>
class Mongo_Data_Source: public Data_Source<T>
{
    public:

        Mongo_Data_Source(): client_(0) {}
        virtual ~Mongo_Data_Source() { delete client_; }

        void request_data(T& data);
        void connect(const settings::Db_Configuration& config);


    private:

        mongocxx::client* client_;
};


}}

#endif // MONGO_DATA_SOURCE_H
