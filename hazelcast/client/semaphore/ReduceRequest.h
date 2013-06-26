//
// Created by sancar koyunlu on 6/25/13.
// Copyright (c) 2013 hazelcast. All rights reserved.



#ifndef HAZELCAST_ReduceRequest
#define HAZELCAST_ReduceRequest


#include "../serialization/SerializationConstants.h"
#include "SemaphorePortableHook.h"
#include "SemaphoreRequest.h"
#include <string>

namespace hazelcast {
    namespace client {
        namespace semaphore {
            class ReduceRequest : public SemaphoreRequest {
                ReduceRequest(const std::string& instanceName, int permitCount)
                : SemaphoreRequest(instanceName, permitCount) {

                };

                int getClassId() const {
                    return SemaphorePortableHook::REDUCE;
                };

                template<typename HzWriter>
                void writePortable(HzWriter& writer) const {
                    SemaphoreRequest::writePortable(writer);
                };

                template<typename HzReader>
                void readPortable(HzReader& reader) {
                    SemaphoreRequest::readPortable(reader);
                };
            };
        }
    }
}

#endif //HAZELCAST_ReduceRequest