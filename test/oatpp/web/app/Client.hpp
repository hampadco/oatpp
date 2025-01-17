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

#ifndef oatpp_test_web_app_Client_hpp
#define oatpp_test_web_app_Client_hpp

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace oatpp { namespace test { namespace web { namespace app {
  
class Client : public oatpp::web::client::ApiClient {
#include OATPP_CODEGEN_BEGIN(ApiClient)
  
  API_CLIENT_INIT(Client)
  
  API_CALL("GET", "/", getRoot)
  API_CALL("GET", "params/{param}", getWithParams, PATH(String, param))
  API_CALL("GET", "queries", getWithQueries, QUERY(String, name), QUERY(Int32, age))
  API_CALL("GET", "queries/map", getWithQueriesMap, QUERY(String, key1), QUERY(Int32, key2), QUERY(Float32, key3))
  API_CALL("GET", "headers", getWithHeaders, HEADER(String, param, "X-TEST-HEADER"))
  API_CALL("POST", "body", postBody, BODY_STRING(String, body))
  API_CALL("POST", "echo", echoBody, BODY_STRING(String, body))
  
#include OATPP_CODEGEN_END(ApiClient)
};
  
}}}}

#endif /* oatpp_test_web_app_Client_hpp */
