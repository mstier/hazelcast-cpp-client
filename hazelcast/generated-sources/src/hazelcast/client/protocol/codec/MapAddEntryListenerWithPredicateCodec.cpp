/*
 * Copyright (c) 2008-2017, Hazelcast, Inc. All Rights Reserved.
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

#include "hazelcast/util/Util.h"
#include "hazelcast/util/ILogger.h"

#include "hazelcast/client/protocol/codec/MapAddEntryListenerWithPredicateCodec.h"
#include "hazelcast/client/exception/UnexpectedMessageTypeException.h"
#include "hazelcast/client/protocol/EventMessageConst.h"

namespace hazelcast {
    namespace client {
        namespace protocol {
            namespace codec {
                const MapMessageType MapAddEntryListenerWithPredicateCodec::RequestParameters::TYPE = HZ_MAP_ADDENTRYLISTENERWITHPREDICATE;
                const bool MapAddEntryListenerWithPredicateCodec::RequestParameters::RETRYABLE = false;
                const int32_t MapAddEntryListenerWithPredicateCodec::ResponseParameters::TYPE = 104;

                MapAddEntryListenerWithPredicateCodec::~MapAddEntryListenerWithPredicateCodec() {
                }

                std::auto_ptr<ClientMessage> MapAddEntryListenerWithPredicateCodec::RequestParameters::encode(
                        const std::string &name, 
                        const serialization::pimpl::Data &predicate, 
                        bool includeValue, 
                        int32_t listenerFlags, 
                        bool localOnly) {
                    int32_t requiredDataSize = calculateDataSize(name, predicate, includeValue, listenerFlags, localOnly);
                    std::auto_ptr<ClientMessage> clientMessage = ClientMessage::createForEncode(requiredDataSize);
                    clientMessage->setMessageType((uint16_t)MapAddEntryListenerWithPredicateCodec::RequestParameters::TYPE);
                    clientMessage->setRetryable(RETRYABLE);
                    clientMessage->set(name);
                    clientMessage->set(predicate);
                    clientMessage->set(includeValue);
                    clientMessage->set(listenerFlags);
                    clientMessage->set(localOnly);
                    clientMessage->updateFrameLength();
                    return clientMessage;
                }

                int32_t MapAddEntryListenerWithPredicateCodec::RequestParameters::calculateDataSize(
                        const std::string &name, 
                        const serialization::pimpl::Data &predicate, 
                        bool includeValue, 
                        int32_t listenerFlags, 
                        bool localOnly) {
                    int32_t dataSize = ClientMessage::HEADER_SIZE;
                    dataSize += ClientMessage::calculateDataSize(name);
                    dataSize += ClientMessage::calculateDataSize(predicate);
                    dataSize += ClientMessage::calculateDataSize(includeValue);
                    dataSize += ClientMessage::calculateDataSize(listenerFlags);
                    dataSize += ClientMessage::calculateDataSize(localOnly);
                    return dataSize;
                }

                MapAddEntryListenerWithPredicateCodec::ResponseParameters::ResponseParameters(ClientMessage &clientMessage) {
                    if (TYPE != clientMessage.getMessageType()) {
                        throw exception::UnexpectedMessageTypeException("MapAddEntryListenerWithPredicateCodec::ResponseParameters::decode", clientMessage.getMessageType(), TYPE);
                    }

                    response = clientMessage.get<std::string >();
                }

                MapAddEntryListenerWithPredicateCodec::ResponseParameters MapAddEntryListenerWithPredicateCodec::ResponseParameters::decode(ClientMessage &clientMessage) {
                    return MapAddEntryListenerWithPredicateCodec::ResponseParameters(clientMessage);
                }

                MapAddEntryListenerWithPredicateCodec::ResponseParameters::ResponseParameters(const MapAddEntryListenerWithPredicateCodec::ResponseParameters &rhs) {
                        response = rhs.response;
                }

                //************************ EVENTS START*************************************************************************//
                MapAddEntryListenerWithPredicateCodec::AbstractEventHandler::~AbstractEventHandler() {
                }

                void MapAddEntryListenerWithPredicateCodec::AbstractEventHandler::handle(std::auto_ptr<protocol::ClientMessage> clientMessage) {
                    int messageType = clientMessage->getMessageType();
                    switch (messageType) {
                        case protocol::EVENT_ENTRY:
                        {
                            std::auto_ptr<serialization::pimpl::Data > key = clientMessage->getNullable<serialization::pimpl::Data >();

                            std::auto_ptr<serialization::pimpl::Data > value = clientMessage->getNullable<serialization::pimpl::Data >();

                            std::auto_ptr<serialization::pimpl::Data > oldValue = clientMessage->getNullable<serialization::pimpl::Data >();

                            std::auto_ptr<serialization::pimpl::Data > mergingValue = clientMessage->getNullable<serialization::pimpl::Data >();

                            int32_t eventType = clientMessage->get<int32_t >();
                            
                            std::string uuid = clientMessage->get<std::string >();
                            
                            int32_t numberOfAffectedEntries = clientMessage->get<int32_t >();
                            
                            handleEntry(key, value, oldValue, mergingValue, eventType, uuid, numberOfAffectedEntries);
                            break;
                        }
                        default:
                            char buf[300];
                            util::snprintf(buf, 300, "[MapAddEntryListenerWithPredicateCodec::AbstractEventHandler::handle] Unknown message type (%d) received on event handler.", clientMessage->getMessageType());
                            util::ILogger::getLogger().warning(buf);
                    }
                }
                //************************ EVENTS END **************************************************************************//

                MapAddEntryListenerWithPredicateCodec::MapAddEntryListenerWithPredicateCodec (const std::string &name, const serialization::pimpl::Data &predicate, const bool &includeValue, const int32_t &listenerFlags, const bool &localOnly)
                        : name_(name), predicate_(predicate), includeValue_(includeValue), listenerFlags_(listenerFlags), localOnly_(localOnly) {
                }

                //************************ IAddListenerCodec interface start ************************************************//
                std::auto_ptr<ClientMessage> MapAddEntryListenerWithPredicateCodec::encodeRequest() const {
                    return RequestParameters::encode(name_, predicate_, includeValue_, listenerFlags_, localOnly_);
                }

                std::string MapAddEntryListenerWithPredicateCodec::decodeResponse(ClientMessage &responseMessage) const {
                    return ResponseParameters::decode(responseMessage).response;
                }
                //************************ IAddListenerCodec interface ends *************************************************//

            }
        }
    }
}

