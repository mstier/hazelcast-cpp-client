//
// Created by sancar koyunlu on 5/23/13.
// Copyright (c) 2013 hazelcast. All rights reserved.
#ifndef HAZELCAST_MAP_TRY_PUT_REQUEST
#define HAZELCAST_MAP_TRY_PUT_REQUEST

#include "../serialization/Data.h"
#include "PortableHook.h"

namespace hazelcast {
    namespace client {
        namespace map {
            class TryPutRequest {
            public:
                TryPutRequest(const std::string& name, serialization::Data& key, serialization::Data& value, int threadId, long timeout)
                :name(name)
                , key(key)
                , value(value)
                , threadId(threadId)
                , ttl(-1)
                , timeout(timeout) {

                };

                int getSerializerId() const {
                    return serialization::SerializationConstants::CONSTANT_TYPE_PORTABLE;
                };

                int getFactoryId() const {
                    return PortableHook::F_ID;
                }

                int getClassId() const {
                    return PortableHook::TRY_PUT;
                }

                template<typename HzWriter>
                void writePortable(HzWriter& writer) const {
                    writer.writeLong("timeout", timeout);
                    writer.writeUTF("n", name);
                    writer.writeInt("t", threadId);
                    writer.writeLong("ttl", ttl);
                    serialization::BufferedDataOutput *out = writer.getRawDataOutput();
                    key.writeData(*out);
                    value.writeData(*out);
                };

                template<typename HzReader>
                void readPortable(HzReader& reader) {
                    timeout = reader.readLong("timeout");
                    name = reader.readUTF("n");
                    threadId = reader.readInt("t");
                    ttl = reader.readLong("ttl");
                    serialization::BufferedDataInput *in = reader.getRawDataInput();
                    key.readData(*in);
                    value.readData(*in);
                };
            private:
                serialization::Data& key;
                serialization::Data& value;
                std::string name;
                int threadId;
                long ttl;
                long timeout;
            };
        }
    }
}

#endif //HAZELCAST_MAP_PUT_REQUEST