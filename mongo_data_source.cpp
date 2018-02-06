#include <time.h>
#include <iostream>

#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <QCoreApplication>

#include <mongo_data_source.h>
#include <utils.h>

namespace fpgui { namespace data_source {

static mongocxx::instance instance{};

static void connect(mongocxx::client** client, std::string& db_name, std::string& db_collection_name, const settings::Db_Configuration& config)
{
    unsigned char key[8] = {0};
    generic_utils::crypto::generate_encryption_key(key);

    std::string pass(config.password);
    if (generic_utils::crypto::is_string_encrypted(pass, key))
        pass = generic_utils::crypto::decrypt_string(pass, key);

    mongocxx::uri uri("mongodb://" + config.username + ":" + pass + "@" + config.hostname + ":" +
                      std::to_string(config.port) + "/?authSource=" + config.auth_db);
    delete *client;
    *client = new mongocxx::client(uri);

    std::vector<std::string> collection = generic_utils::tokenize(config.collection.c_str(), '.');

    if (collection.size() != 2)
        THROWM(exceptions::Incorrect_Parameter, QCoreApplication::translate("Generic_Exception", "Collection parameter in app configuration is malformed."));

    db_name = collection[0];
    db_collection_name = collection[1];

    bool found = false;

    auto dbs((*client)->list_databases());
    for (const bsoncxx::document::view& db : dbs)
        if (db["name"].get_utf8().value.to_string() == db_name)
        {
            found = true;
            break;
        }

    if (!found)
        THROWM(exceptions::Incorrect_Parameter, QCoreApplication::translate("Generic_Exception", "Database not found."));

    found = false;
    mongocxx::database fplog = (**client)[db_name];
    auto colls(fplog.list_collections());
    for (const bsoncxx::document::view& coll : colls)
        if (coll["name"].get_utf8().value.to_string() == db_collection_name)
        {
            found = true;
            break;
        }

    if (!found)
        THROWM(exceptions::Incorrect_Parameter, QCoreApplication::translate("Generic_Exception", "Collection not found."));
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
