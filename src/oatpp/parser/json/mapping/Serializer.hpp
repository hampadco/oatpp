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

#ifndef oatpp_parser_json_mapping_Serializer_hpp
#define oatpp_parser_json_mapping_Serializer_hpp

#include "oatpp/core/data/mapping/type/ListMap.hpp"
#include "oatpp/core/data/mapping/type/List.hpp"
#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/data/mapping/type/Type.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

#include "oatpp/core/parser/Caret.hpp"

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {
  
class Serializer {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::Type::Property Property;
  typedef oatpp::data::mapping::type::Type::Properties Properties;
  
  typedef oatpp::data::mapping::type::Object Object;
  typedef oatpp::String String;
  
  template<class T>
  using PolymorphicWrapper = data::mapping::type::PolymorphicWrapper<T>;
  
  typedef oatpp::data::mapping::type::AbstractObjectWrapper AbstractObjectWrapper;
  typedef oatpp::data::mapping::type::List<AbstractObjectWrapper> AbstractList;
  typedef oatpp::data::mapping::type::ListMap<String, AbstractObjectWrapper> AbstractFieldsMap;
  
public:
  
  class Config : public oatpp::base::Countable {
  public:
    Config()
    {}
  public:
    
    static std::shared_ptr<Config> createShared(){
      return std::make_shared<Config>();
    }
    
    bool includeNullFields = true;
    bool throwOnUnknownTypes = true;
    
  };
  
private:
  
  static void writeString(oatpp::data::stream::OutputStream* stream, p_char8 data, v_int32 size);
  static void writeString(oatpp::data::stream::OutputStream* stream, const char* data);
  
  template<class T>
  static void writeSimpleData(oatpp::data::stream::OutputStream* stream, const PolymorphicWrapper<T>& value){
    if(value){
      stream->writeAsString(value.get()->getValue());
    } else {
      stream->write("null", 4);
    }
  }
  
  static void writeList(oatpp::data::stream::OutputStream* stream, const AbstractList::ObjectWrapper& list, const std::shared_ptr<Config>& config);
  static void writeFieldsMap(oatpp::data::stream::OutputStream* stream, const AbstractFieldsMap::ObjectWrapper& map, const std::shared_ptr<Config>& config);
  static void writeObject(oatpp::data::stream::OutputStream* stream, const PolymorphicWrapper<Object>& polymorph, const std::shared_ptr<Config>& config);
  
  static void writeValue(oatpp::data::stream::OutputStream* stream, const AbstractObjectWrapper& polymorph, const std::shared_ptr<Config>& config);
  
public:
  
  static void serialize(const std::shared_ptr<oatpp::data::stream::OutputStream>& stream,
                        const oatpp::data::mapping::type::AbstractObjectWrapper& polymorph,
                        const std::shared_ptr<Config>& config){
    auto type = polymorph.valueType;
    if(type->name == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
      writeObject(stream.get(), oatpp::data::mapping::type::static_wrapper_cast<Object>(polymorph), config);
    } else if(type->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
      writeList(stream.get(), oatpp::data::mapping::type::static_wrapper_cast<AbstractList>(polymorph), config);
    } else if(type->name == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME) {
      writeFieldsMap(stream.get(), oatpp::data::mapping::type::static_wrapper_cast<AbstractFieldsMap>(polymorph), config);
    } else {
      throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::serialize()]: Unknown parameter type");
    }
  }
  
};
  
}}}}

#endif /* oatpp_parser_json_mapping_Serializer_hpp */
