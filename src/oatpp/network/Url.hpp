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

#ifndef oatpp_network_Url_hpp
#define oatpp_network_Url_hpp

#include "oatpp/core/data/share/MemoryLabel.hpp"
#include "oatpp/core/parser/Caret.hpp"
#include "oatpp/core/collection/ListMap.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>

namespace oatpp { namespace network {


// TODO - refactor to use oatpp::data::share::MemoryLabel
class Url : public oatpp::base::Countable {
public:
  typedef oatpp::data::share::StringKeyLabel StringKeyLabel;
public:
  typedef std::unordered_map<oatpp::String, oatpp::String> Parameters;
  typedef std::unordered_map<StringKeyLabel, StringKeyLabel> ParametersAsLabels;
public:
  
  struct Authority {
    oatpp::String userInfo;
    oatpp::String host;
    v_int32 port = -1;
  };
  
public:
  
  class Parser {
  public:
    
    /**
     *  parse "<scheme>:"
     *  example "http", "https", "ftp"
     *  returns lowercase string before ':' char
     *  caret should be at the first char of the scheme
     */
    static oatpp::String parseScheme(oatpp::parser::Caret& caret);
    
    /**
     *  parse url authority components.
     *  userinfo is not parsed into login and password separately as
     *  inclusion of password in userinfo is deprecated and ignored here
     *  caret should be at the first char of the authority (not at "//")
     */
    static Url::Authority parseAuthority(oatpp::parser::Caret& caret);
    
    /**
     *  parse path of the url
     *  caret should be at the first char of the path
     */
    static oatpp::String parsePath(oatpp::parser::Caret& caret);
    
    /**
     *  parse query params in form of "?<paramName>=<paramValue>&<paramName>=<paramValue>..." referred by ParsingCaret
     *  and put that params to Parameters map
     */
    static void parseQueryParamsToMap(Url::Parameters& params, oatpp::parser::Caret& caret);
    
    /**
     *  parse query params in form of "?<paramName>=<paramValue>&<paramName>=<paramValue>..." referred by str
     *  and put that params to Parameters map
     */
    static void parseQueryParamsToMap(Url::Parameters& params, const oatpp::String& str);
    
    /**
     *  parse query params in form of "?<paramName>=<paramValue>&<paramName>=<paramValue>..." referred by ParsingCaret
     */
    static Url::Parameters parseQueryParams(oatpp::parser::Caret& caret);
    
    /**
     *  parse query params in form of "?<paramName>=<paramValue>&<paramName>=<paramValue>..." referred by str
     */
    static Url::Parameters parseQueryParams(const oatpp::String& str);

    /**
     * Same as parseQueryParams() but use StringKeyLabel instead of a String.
     * Zero allocations. Use this method for better performance.
     * @param str
     * @return std::unordered_map<StringKeyLabel, StringKeyLabel>
     */
    static ParametersAsLabels labelQueryParams(const oatpp::String& str);

    /**
     * Parse Url
     * @param caret
     * @return parsed URL structure
     */
    static Url parseUrl(oatpp::parser::Caret& caret);

    /**
     * Parse Url
     * @param str
     * @return parsed URL structure
     */
    static Url parseUrl(const oatpp::String& str);
    
  };
  
public:
  
  oatpp::String scheme;
  Authority authority;
  oatpp::String path;
  Parameters queryParams;
  
};
  
}}

#endif /* oatpp_network_url_Url_hpp */
