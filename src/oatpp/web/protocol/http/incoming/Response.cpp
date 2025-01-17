/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "./Response.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace incoming {

Response::Response(v_int32 statusCode,
                   const oatpp::String& statusDescription,
                   const http::Protocol::Headers& headers,
                   const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                   const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder)
  : m_statusCode(statusCode)
  , m_statusDescription(statusDescription)
  , m_headers(headers)
  , m_bodyStream(bodyStream)
  , m_bodyDecoder(bodyDecoder)
{}

std::shared_ptr<Response> Response::createShared(v_int32 statusCode,
                                                 const oatpp::String& statusDescription,
                                                 const http::Protocol::Headers& headers,
                                                 const std::shared_ptr<oatpp::data::stream::InputStream>& bodyStream,
                                                 const std::shared_ptr<const http::incoming::BodyDecoder>& bodyDecoder) {
  return Shared_Incoming_Response_Pool::allocateShared(statusCode, statusDescription, headers, bodyStream, bodyDecoder);
}

v_int32 Response::getStatusCode() const {
  return m_statusCode;
}

oatpp::String Response::getStatusDescription() const {
  return m_statusDescription;
}

const http::Protocol::Headers& Response::getHeaders() const {
  return m_headers;
}

std::shared_ptr<oatpp::data::stream::InputStream> Response::getBodyStream() const {
  return m_bodyStream;
}

std::shared_ptr<const http::incoming::BodyDecoder> Response::getBodyDecoder() const {
  return m_bodyDecoder;
}

void Response::streamBody(const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
  m_bodyDecoder->decode(m_headers, m_bodyStream, toStream);
}

oatpp::String Response::readBodyToString() const {
  return m_bodyDecoder->decodeToString(m_headers, m_bodyStream);
}

oatpp::async::Action Response::streamBodyAsync(oatpp::async::AbstractCoroutine* parentCoroutine,
                                     const oatpp::async::Action& actionOnReturn,
                                     const std::shared_ptr<oatpp::data::stream::OutputStream>& toStream) const {
  return m_bodyDecoder->decodeAsync(parentCoroutine, actionOnReturn, m_headers, m_bodyStream, toStream);
}

}}}}}
