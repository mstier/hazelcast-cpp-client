//
// Created by sancar koyunlu on 5/23/13.
// Copyright (c) 2013 hazelcast. All rights reserved.
#ifndef HAZELCAST_MAP_PUT_REQUEST
#define HAZELCAST_MAP_PUT_REQUEST

#include "../serialization/Data.h"
#include "RequestIDs.h"

namespace hazelcast {
    namespace client {
        namespace map {
            class PutRequest {
            public:
                PutRequest(const std::string& name, serialization::Data& key, serialization::Data& value, int threadId, long ttl)
                :name(name)
                , key(key)
                , value(value)
                , threadId(threadId)
                , ttl(ttl) {

                };

                int getTypeSerializerId() const {
                    return serialization::SerializationConstants::CONSTANT_TYPE_PORTABLE;
                };

                int getFactoryId() const {
                    return map::RequestIDs::F_ID;
                }

                int getClassId() const {
                    return map::RequestIDs::PUT;
                }

                template<typename HzWriter>
                void writePortable(HzWriter& writer) const {
                    writer["n"] << name;
                    writer["t"] << threadId;
                    writer["ttl"] << ttl;
                    writer << key;
                    writer << value;
                };

                template<typename HzReader>
                void readPortable(HzReader& reader) {
                    reader["n"] >> name;
                    reader["t"] >> threadId;
                    reader["ttl"] >> ttl;
                    reader >> key;
                    reader >> value;
                };
            private:
                serialization::Data& key;
                serialization::Data& value;
                std::string name;
                int threadId;
                long ttl;
            };
        }
    }
}

#endif //HAZELCAST_MAP_PUT_REQUEST
