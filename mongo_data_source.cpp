#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include <mongo_data_source.h>

namespace fpgui { namespace data_source {

static mongocxx::instance instance{};

static void connect(mongocxx::client** client, const settings::Db_Configuration& config)
{
    mongocxx::uri uri("mongodb://localhost:27017");
    delete *client;
    *client = new mongocxx::client(uri);
}

template <> void Mongo_Data_Source<std::queue<std::string>>::connect(const settings::Db_Configuration& config)
{
    data_source::connect(&client_, config);
}

template <> void Mongo_Data_Source<std::queue<std::string>>::request_data(std::queue<std::string>& data)
{
    mongocxx::database fplog = (*client_)["fplog"];
    mongocxx::collection logs = fplog["logs"];

    mongocxx::cursor cur = logs.find(bsoncxx::builder::stream::document{} << bsoncxx::builder::stream::finalize);
    for (const bsoncxx::document::view& doc: cur)
    {
    }
}

}}
