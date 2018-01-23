#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

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
    static std::string last_id;

    mongocxx::database fplog = (*client_)["fplog"];
    mongocxx::collection logs = fplog["logs"];

    std::unique_ptr<mongocxx::cursor> cur;

    if (last_id.empty())
        cur.reset(new mongocxx::cursor(logs.find(bsoncxx::builder::stream::document{} << bsoncxx::builder::stream::finalize)));
    else
        cur.reset(new mongocxx::cursor(
                      logs.find(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::builder::stream::open_document <<
                                "$gt" << bsoncxx::oid(last_id) << bsoncxx::builder::stream::close_document <<
                                bsoncxx::builder::stream::finalize)));

    for (const bsoncxx::document::view& doc: *cur)
    {
        const bsoncxx::document::element& id = doc["_id"];
        last_id = id.get_oid().value.to_string();
        data.push(bsoncxx::to_json(doc));
    }
}

}}
