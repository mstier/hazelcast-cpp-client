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
//
//  Created by ihsan demir on 12 Jan 2017
//
#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <WinSock2.h>
#endif

#include "hazelcast/util/SyncHttpClient.h"
#include "hazelcast/client/exception/IOException.h"

namespace hazelcast {
    namespace util {
            SyncHttpClient::SyncHttpClient(const std::string &serverIp, const std::string &uriPath)
                    : server(serverIp), uriPath(uriPath), socket(ioService), responseStream(&response) {
            }

            std::istream &SyncHttpClient::openConnection() {
                try {
                    // Get a list of endpoints corresponding to the server name.
                    asio::ip::tcp::resolver resolver(ioService);
                    asio::ip::tcp::resolver::query query(server, "http");
                    asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

                    asio::connect(socket, endpoint_iterator);

                    socket.lowest_layer().set_option(asio::ip::tcp::no_delay(true));

                    // Form the request. We specify the "Connection: close" header so that the
                    // server will close the socket after transmitting the response. This will
                    // allow us to treat all data up until the EOF as the content.
                    asio::streambuf request;
                    std::ostream request_stream(&request);
                    request_stream << "GET " << uriPath << " HTTP/1.0\r\n";
                    request_stream << "Host: " << server << "\r\n";
                    request_stream << "Accept: */*\r\n";
                    request_stream << "Connection: close\r\n\r\n";

                    // Send the request.
                    asio::write(socket, request.data());

                    // Read the response status line. The response streambuf will automatically
                    // grow to accommodate the entire line. The growth may be limited by passing
                    // a maximum size to the streambuf constructor.
                    asio::read_until(socket, response, "\r\n");

                    // Check that response is OK.
                    std::string httpVersion;
                    responseStream >> httpVersion;
                    unsigned int statusCode;
                    responseStream >> statusCode;
                    std::string statusMessage;
                    std::getline(responseStream, statusMessage);
                    if (!responseStream || httpVersion.substr(0, 5) != "HTTP/") {
                        throw client::exception::IOException("openConnection", "Invalid response");
                    }
                    if (statusCode != 200) {
                        std::stringstream out;
                        out << "Response returned with status: " << statusCode << " Status message:" << statusMessage;
                        throw client::exception::IOException("SyncHttpClient::openConnection", out.str());;
                    }

                    // Read the response headers, which are terminated by a blank line.
                    asio::read_until(socket, response, "\r\n\r\n");

                    // Process the response headers.
                    std::string header;
                    while (std::getline(responseStream, header) && header != "\r");

                    // Read until EOF
                    asio::error_code error;
                    size_t bytesRead;
                    while ((bytesRead = asio::read(socket, response.prepare(1024),
                                      asio::transfer_at_least(1), error))) {
                        response.commit(bytesRead);
                    }

                    if (error != asio::error::eof) {
                        throw asio::system_error(error);
                    }

                    return responseStream;
                } catch (asio::system_error &e) {
                    std::ostringstream out;
                    out << "Could not retrieve response from http://" << server << uriPath << " Error:" << e.what();
                    throw client::exception::IOException("SyncHttpClient::openConnection", out.str());
                }
            }
    }
}
