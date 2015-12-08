/*
 * Copyright (c) 2008-2015, Hazelcast, Inc. All Rights Reserved.
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



#include "hazelcast/client/protocol/codec/QueueOfferCodec.h"
#include "hazelcast/client/exception/UnexpectedMessageTypeException.h"
#include "hazelcast/client/serialization/pimpl/Data.h"

namespace hazelcast {
    namespace client {
        namespace protocol {
            namespace codec {
                const QueueMessageType QueueOfferCodec::RequestParameters::TYPE = HZ_QUEUE_OFFER;
                const bool QueueOfferCodec::RequestParameters::RETRYABLE = false;
                const int32_t QueueOfferCodec::ResponseParameters::TYPE = 101;
                std::auto_ptr<ClientMessage> QueueOfferCodec::RequestParameters::encode(
                        const std::string &name, 
                        const serialization::pimpl::Data &value, 
                        int64_t timeoutMillis) {
                    int32_t requiredDataSize = calculateDataSize(name, value, timeoutMillis);
                    std::auto_ptr<ClientMessage> clientMessage = ClientMessage::createForEncode(requiredDataSize);
                    clientMessage->setMessageType((uint16_t)QueueOfferCodec::RequestParameters::TYPE);
                    clientMessage->setRetryable(RETRYABLE);
                    clientMessage->set(name);
                    clientMessage->set(value);
                    clientMessage->set(timeoutMillis);
                    clientMessage->updateFrameLength();
                    return clientMessage;
                }

                int32_t QueueOfferCodec::RequestParameters::calculateDataSize(
                        const std::string &name, 
                        const serialization::pimpl::Data &value, 
                        int64_t timeoutMillis) {
                    int32_t dataSize = ClientMessage::HEADER_SIZE;
                    dataSize += ClientMessage::calculateDataSize(name);
                    dataSize += ClientMessage::calculateDataSize(value);
                    dataSize += ClientMessage::calculateDataSize(timeoutMillis);
                    return dataSize;
                }

                QueueOfferCodec::ResponseParameters::ResponseParameters(ClientMessage &clientMessage) {
                    if (TYPE != clientMessage.getMessageType()) {
                        throw exception::UnexpectedMessageTypeException("QueueOfferCodec::ResponseParameters::decode", clientMessage.getMessageType(), TYPE);
                    }

                    response = clientMessage.get<bool >();
                }

                QueueOfferCodec::ResponseParameters QueueOfferCodec::ResponseParameters::decode(ClientMessage &clientMessage) {
                    return QueueOfferCodec::ResponseParameters(clientMessage);
                }

                QueueOfferCodec::ResponseParameters::ResponseParameters(const QueueOfferCodec::ResponseParameters &rhs) {
                        response = rhs.response;
                }
                //************************ EVENTS END **************************************************************************//

            }
        }
    }
}

