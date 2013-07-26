//
// Created by sancar koyunlu on 5/23/13.
// Copyright (c) 2013 hazelcast. All rights reserved.
#ifndef HAZELCAST_QUEUE_ADD_ALL_REQUEST
#define HAZELCAST_QUEUE_ADD_ALL_REQUEST

#include "../serialization/Data.h"
#include "QueuePortableHook.h"

namespace hazelcast {
    namespace client {
        namespace queue {
            class AddAllRequest : public Portable {
            public:

                AddAllRequest(const std::string& name, std::vector<serialization::Data>& dataList)
                :name(name)
                , dataList(dataList) {

                };

                int getFactoryId() const {
                    return queue::QueuePortableHook::F_ID;
                }

                int getClassId() const {
                    return queue::QueuePortableHook::ADD_ALL;
                }

                template<typename HzWriter>
                void writePortable(HzWriter& writer) const {
                    writer.writeUTF("n", name);
                    writer.writeInt("s", dataList.size());
                    serialization::BufferedDataOutput *out = writer.getRawDataOutput();
                    for (int i = 0; i < dataList.size(); ++i) {
                        dataList[i].writeData(*out);
                    }
                };

                template<typename HzReader>
                void readPortable(HzReader& reader) {
                    name = reader.readUTF("n");
                    int size = reader.readInt("s");
                    dataList.resize(size);
                    serialization::BufferedDataInput *in = reader.getRawDataInput();
                    for (int i = 0; i < size; ++i) {
                        dataList[i].readData(*in);
                    }
                };
            private:
                std::vector<serialization::Data>& dataList;
                std::string name;
            };
        }
    }
}

#endif //HAZELCAST_QUEUE_ADD_ALL_REQUEST