//
// Created by sancar koyunlu on 5/23/13.
// Copyright (c) 2013 hazelcast. All rights reserved.
#ifndef HAZELCAST_MAP_CONTAINS_KEY_R
#define HAZELCAST_MAP_CONTAINS_KEY_R

#include "../serialization/Data.h"
#include "RequestIDs.h"

namespace hazelcast {
    namespace client {
        namespace map {
            class ContainsKeyRequest {
            public:
                ContainsKeyRequest(const std::string& name, serialization::Data& key)
                :name(name)
                , key(key) {
                };

                int getTypeSerializerId() const {
                    return serialization::SerializationConstants::CONSTANT_TYPE_PORTABLE;
                };

                int getFactoryId() const {
                    return map::RequestIDs::F_ID;
                }

                int getClassId() const {
                    return map::RequestIDs::CONTAINS_KEY;
                }

                template<typename HzWriter>
                void writePortable(HzWriter& writer) const {
                    writer["n"] << name;
                    writer << key;
                };

                template<typename HzReader>
                void readPortable(HzReader& reader) {
                    reader["n"] >> name;
                    reader >> key;
                };
            private:
                serialization::Data& key;
                std::string name;
            };
        }
    }
}

#endif //HAZELCAST_MAP_PUT_REQUEST
