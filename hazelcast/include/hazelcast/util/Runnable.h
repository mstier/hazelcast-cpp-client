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

#ifndef HAZELCAST_UTIL_RUNNABLE_H_
#define HAZELCAST_UTIL_RUNNABLE_H_

#include <stdint.h>

#include "hazelcast/util/Named.h"

namespace hazelcast {
    namespace util {
        class HAZELCAST_API Runnable : public Named {
        public:
            virtual ~Runnable();

            virtual void run() = 0;

            virtual bool isStriped();
        };

        class HAZELCAST_API StripedRunnable : public Runnable {
        public:
            virtual int32_t getKey() = 0;

            virtual bool isStriped();
        };

        class HAZELCAST_API RunnableDelegator : public Runnable {
        public:
            RunnableDelegator(Runnable &runnable);

            virtual void run();

            virtual const std::string getName() const;

        private:
            Runnable &runnable;
        };
    }
}


#endif //HAZELCAST_UTIL_RUNNABLE_H_