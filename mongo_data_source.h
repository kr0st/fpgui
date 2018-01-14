#ifndef MONGO_DATA_SOURCE_H
#define MONGO_DATA_SOURCE_H

#include <data_source.h>

namespace fpgui { namespace data_source {

template <typename T=std::queue<std::string>>
class Mongo_Data_Source: public Data_Source<T>
{
    public:

        void request_data(T &data)
        {
        }


    private:
};


}}

#endif // MONGO_DATA_SOURCE_H
