//
//  BufferedDataInput.h
//  Server
//
//  Created by sancar koyunlu on 1/3/13.
//  Copyright (c) 2013 sancar koyunlu. All rights reserved.
//

#ifndef HAZELCAST_DATA_INPUT
#define HAZELCAST_DATA_INPUT

#include "../HazelcastException.h"
#include <vector>
#include <string>

namespace hazelcast {
    namespace client {
        namespace serialization {

            class SerializationService;

            class SerializationContext;

            typedef unsigned char byte;

            class BufferedDataInput {
            public:

                BufferedDataInput(const std::vector<byte>&);

                BufferedDataInput& operator [](std::string);

                ~BufferedDataInput();

                void readFully(std::vector<byte>&);

                void readFully(byte *bytes, int off, int len);

                int skipBytes(int i);

                bool readBoolean();

                byte readByte();

                short readShort();

                char readChar();

                int readInt();

                long readLong();

                float readFloat();

                double readDouble();

                std::string readUTF();

                std::vector<byte> readByteArray();

                std::vector<char> readCharArray();

                std::vector<int> readIntArray();

                std::vector<long> readLongArray();

                std::vector<double> readDoubleArray();

                std::vector<float> readFloatArray();

                std::vector<short> readShortArray();

                int position();

                void position(int newPos);

            private:
                byte *ptr;
                byte *beg;

                static int const STRING_CHUNK_SIZE = 16 * 1024;

                std::string readShortUTF();

                BufferedDataInput& operator = (const BufferedDataInput&);

            };

            template<typename T>
            inline void readPortable(BufferedDataInput& in, const std::vector<T>& data) {
                throw hazelcast::client::HazelcastException("template<typename T>\n"
                        "            inline void readPortable(BufferedDataInput& in, std::vector<T>& data) >> Not supported");
            };

            template<typename T>
            inline void operator >>(BufferedDataInput& dataInput, T& data) {
                readPortable(dataInput, data);
            };
        }
    }
}
#endif /* HAZELCAST_DATA_INPUT */
