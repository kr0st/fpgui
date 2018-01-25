#include <time.h>
#include <iostream>

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongo_data_source.h>
#include <utils.h>

namespace fpgui { namespace data_source {

static mongocxx::instance instance{};

static void connect(mongocxx::client** client, std::string& db_name, std::string& db_collection_name, const settings::Db_Configuration& config)
{
    mongocxx::uri uri("mongodb://localhost:27017");
    delete *client;
    *client = new mongocxx::client(uri);

    std::vector<std::string> collection = generic_utils::tokenize(config.collection.c_str(), '.');

    //TODO: exception handling!!
    db_name = collection[0];
    db_collection_name = collection[1];
}

template <> void Mongo_Data_Source<std::queue<std::string>>::connect(const settings::Db_Configuration& config)
{
    data_source::connect(&client_, db_name_, db_collection_name_, config);
}

std::string generate_first_oid()
{
    long long t = std::time(0);
    char s[100] = {0};
    sprintf(s, "%8llx0000000000000000", t);
    return (s);
}

static std::string g_last_id = generate_first_oid();

mongocxx::cursor* request_data(mongocxx::client* client, const std::string& db_name, const std::string& db_collection_name)
{
    mongocxx::database fplog = (*client)[db_name];
    mongocxx::collection logs = fplog[db_collection_name];

    mongocxx::cursor* cur = new mongocxx::cursor(logs.find(bsoncxx::builder::stream::document{} << "_id" <<
                                                           bsoncxx::builder::stream::open_document <<
                                                           "$gt" << bsoncxx::oid(g_last_id) <<
                                                           bsoncxx::builder::stream::close_document <<
                                                           bsoncxx::builder::stream::finalize));
    return cur;
}

template <> void Mongo_Data_Source<std::queue<std::string>>::request_data(std::queue<std::string>& data)
{
    std::unique_ptr<mongocxx::cursor> cur(data_source::request_data(client_, db_name_, db_collection_name_));

    for (const bsoncxx::document::view& doc: *cur)
    {
        g_last_id = doc["_id"].get_oid().value.to_string();
        data.push(bsoncxx::to_json(doc));
    }
}

}}
