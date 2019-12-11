/*
 * Copyright (c) 2008-2019, Hazelcast, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HAZElCAST_CLIENT_CLIENT_H_
#define HAZElCAST_CLIENT_CLIENT_H_

#include "hazelcast/client/Endpoint.h"

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4251) //for dll export
#endif

namespace hazelcast {
    namespace client {
        /**
         * The Client interface allows to get information about
         * a connected client's socket address, type and UUID.
         *
         */
        class HAZELCAST_API Client : public Endpoint {
        public:
            Client(const boost::shared_ptr<std::string> &uuid, const boost::shared_ptr<Address> &socketAddress,
                   const std::string &name);

            const std::string &getName() const;

        private:
            std::string name;
        };
    }
}

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#pragma warning(pop)
#endif

#endif //HAZElCAST_CLIENT_CLIENT_H_

