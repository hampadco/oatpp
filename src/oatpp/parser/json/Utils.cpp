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

#include "Utils.hpp"

#include "oatpp/encoding/Unicode.hpp"
#include "oatpp/encoding/Hex.hpp"

namespace oatpp { namespace parser { namespace json{

const char* const Utils::ERROR_INVALID_ESCAPED_CHAR = "ERROR_INVALID_ESCAPED_CHAR";
const char* const Utils::ERROR_INVALID_SURROGATE_PAIR = "ERROR_INVALID_SURROGATE_PAIR";
const char* const Utils::ERROR_PARSER_QUOTE_EXPECTED = "'\"' - EXPECTED";

v_int32 Utils::calcEscapedStringSize(p_char8 data, v_int32 size, v_int32& safeSize) {
  v_int32 result = 0;
  v_int32 i = 0;
  safeSize = size;
  while (i < size) {
    v_char8 a = data[i];
    if(a < 32) {
      i ++;
      if(a == '\b' || a == '\f' || a == '\n' || a == '\r' || a == '\t'){
        result += 2; // '\n'
      } else {
        result += 6; // '\uFFFF' - 6 chars
      }
    } else if(a < 128){
      i ++;
      if(a == '\"' || a == '\\' || a == '/'){
        result += 2; // '\/'
      } else {
        result ++;
      }
    } else {
      v_int32 charSize = oatpp::encoding::Unicode::getUtf8CharSequenceLength(a);
      if(charSize != 0) {
        if(i + charSize > size) {
          safeSize = i;
        }
        i += charSize;
        if(charSize < 4) {
          result += 6; // '\uFFFF' - 6 chars
        } else if(charSize == 4) {
          result += 12; // '\uFFFF\uFFFF' - 12 chars surrogate pair
        } else {
          result += 11; // '\u+FFFFFFFF' - 11 chars NOT JSON standard case
        }
      } else {
        // invalid char
        i ++;
        result ++;
      }
    }
  }
  return result;
}

v_int32 Utils::calcUnescapedStringSize(p_char8 data, v_int32 size, const char* & error, v_int32& errorPosition) {
  error = nullptr;
  v_int32 result = 0;
  v_int32 i = 0;
  
  while (i < size) {
    v_char8 a = data[i];
    if(a == '\\'){
      
      if(i + 1 == size){
        error = ERROR_INVALID_ESCAPED_CHAR;
        errorPosition = i;
        return 0;
      }
      
      v_char8 b = data[i + 1];
      
      if(b == '"' || b == '\\' || b == '/' || b == 'b' || b == 'f' || b == 'n' || b == 'r' || b == 't'){
        result += 1;
        i += 2;
      } else if(b == 'u'){
        
        if(i + 6 > size){
          error = ERROR_INVALID_ESCAPED_CHAR;
          errorPosition = i;
          return 0;
        }
        
        if(data[i + 2] == '+') { // not JSON standard case
          if(i + 11 > size){
            error = ERROR_INVALID_ESCAPED_CHAR;
            errorPosition = i;
            return 0;
          }
          v_word32 code;
          error = encoding::Hex::readWord32(&data[i + 3], code);
          if(error != nullptr){
            errorPosition = i + 3;
            return 0;
          }
          i += 11;
          result += encoding::Unicode::getUtf8CharSequenceLengthForCode(code);
        } else {
          v_word16 code;
          error = encoding::Hex::readWord16(&data[i + 2], code);
          if(error != nullptr){
            errorPosition = i + 2;
            return 0;
          }
          
          if(code >= 0xD800 && code <= 0xDBFF){
            if(i + 12 > size){
              error = ERROR_INVALID_SURROGATE_PAIR;
              errorPosition = i;
              return 0;
            }
            v_word16 low;
            error = encoding::Hex::readWord16(&data[i + 8], low);
            if(error != nullptr){
              errorPosition = i + 8;
              return 0;
            }
            
            if(low >= 0xDC00 && low <= 0xDFFF){
              v_word32 bigCode = encoding::Unicode::utf16SurrogatePairToCode(code, low);
              i += 12;
              result += encoding::Unicode::getUtf8CharSequenceLengthForCode(bigCode);
            } else {
              error = ERROR_INVALID_SURROGATE_PAIR;
              errorPosition = i;
              return 0;
            }
            
          } else {
            i += 6;
            result += encoding::Unicode::getUtf8CharSequenceLengthForCode(code);
          }
        }
        
      } else {
        error = ERROR_INVALID_ESCAPED_CHAR;
        errorPosition = i;
        return 0;
      }
      
    } else {
      i ++;
      result ++;
    }
    
  }
  
  return result;
}
  
v_int32 Utils::escapeUtf8Char(p_char8 sequence, p_char8 buffer){
  v_int32 length;
  v_int32 code = oatpp::encoding::Unicode::encodeUtf8Char(sequence, length);
  if(code < 0x00010000) {
    buffer[0] = '\\';
    buffer[1] = 'u';
    oatpp::encoding::Hex::writeWord16(code, &buffer[2]);
    return 6;
  } else if(code < 0x00200000) {
    v_int16 high;
    v_int16 low;
    oatpp::encoding::Unicode::codeToUtf16SurrogatePair(code, high, low);
    buffer[0] = '\\';
    buffer[1] = 'u';
    oatpp::encoding::Hex::writeWord16(high, &buffer[2]);
    buffer[6] = '\\';
    buffer[7] = 'u';
    oatpp::encoding::Hex::writeWord16(low, &buffer[8]);
    return 12;
  } else {
    buffer[0] = '\\';
    buffer[1] = 'u';
    buffer[2] = '+';
    oatpp::encoding::Hex::writeWord32(code, &buffer[2]);
    return 11;
  }
}
  
oatpp::String Utils::escapeString(p_char8 data, v_int32 size, bool copyAsOwnData) {
  v_int32 safeSize;
  v_int32 escapedSize = calcEscapedStringSize(data, size, safeSize);
  if(escapedSize == size) {
    return String((const char*)data, size, copyAsOwnData);
  }
  auto result = String(escapedSize);
  v_int32 i = 0;
  p_char8 resultData = result->getData();
  v_int32 pos = 0;
  
  while (i < safeSize) {
    v_char8 a = data[i];
    if(a < 32) {
      if(a == '\b'){
        resultData[pos] = '\\'; resultData[pos + 1] = 'b'; pos += 2;
      } else if(a == '\f'){
        resultData[pos] = '\\'; resultData[pos + 1] = 'f'; pos += 2;
      } else if(a == '\n'){
        resultData[pos] = '\\'; resultData[pos + 1] = 'n'; pos += 2;
      } else if(a == '\r'){
        resultData[pos] = '\\'; resultData[pos + 1] = 'r'; pos += 2;
      } else if(a == '\t'){
        resultData[pos] = '\\'; resultData[pos + 1] = 't'; pos += 2;
      } else {
        resultData[pos] = '\\';
        resultData[pos + 1] = 'u';
        oatpp::encoding::Hex::writeWord16(a, &resultData[pos + 2]);
        pos += 6;
      }
      i ++;
    } else if(a < 128){
      if(a == '\"'){
        resultData[pos] = '\\'; resultData[pos + 1] = '"'; pos += 2;
      } else if(a == '\\'){
        resultData[pos] = '\\'; resultData[pos + 1] = '\\'; pos += 2;
      } else if(a == '/'){
        resultData[pos] = '\\'; resultData[pos + 1] = '/'; pos += 2;
      } else {
        resultData[pos] = data[i];
        pos ++;
      }
      i ++;
    } else {
      v_int32 charSize = oatpp::encoding::Unicode::getUtf8CharSequenceLength(a);
      if(charSize != 0) {
        pos += escapeUtf8Char(&data[i], &resultData[pos]);
        i += charSize;
      } else {
        // invalid char
        resultData[pos] = data[i];
        i ++;
        pos ++;
      }
    }
  }
  
  if(size > safeSize){
    for(v_int32 i = pos; i < result->getSize(); i ++){
      resultData[i] = '?';
    }
  }
  
  return result;
}

void Utils::unescapeStringToBuffer(p_char8 data, v_int32 size, p_char8 resultData){
  
  v_int32 i = 0;
  v_int32 pos = 0;
  
  while (i < size) {
    v_char8 a = data[i];
    
    if(a == '\\'){
      v_char8 b = data[i + 1];
      if(b != 'u'){
        switch (b) {
          case '"': resultData[pos] = '"'; pos ++; break;
          case '\\': resultData[pos] = '\\'; pos ++; break;
          case '/': resultData[pos] = '/'; pos ++; break;
          case 'b': resultData[pos] = '\b'; pos ++; break;
          case 'f': resultData[pos] = '\f'; pos ++; break;
          case 'n': resultData[pos] = '\n'; pos ++; break;
          case 'r': resultData[pos] = '\r'; pos ++; break;
          case 't': resultData[pos] = '\t'; pos ++; break;
        }
        i += 2;
      } else {
        if(data[i + 2] == '+'){ // Not JSON standard case
          v_word32 code;
          encoding::Hex::readWord32(&data[i + 3], code);
          i += 11;
          pos += encoding::Unicode::decodeUtf8Char(code, &resultData[pos]);
        } else {
          
          v_word16 code;
          encoding::Hex::readWord16(&data[i + 2], code);
          
          if(code >= 0xD800 && code <= 0xDBFF){
            v_word16 low;
            encoding::Hex::readWord16(&data[i + 8], low);
            v_word32 bigCode = encoding::Unicode::utf16SurrogatePairToCode(code, low);
            pos += encoding::Unicode::decodeUtf8Char(bigCode, &resultData[pos]);
            i += 12;
          } else {
            pos += encoding::Unicode::decodeUtf8Char(code, &resultData[pos]);
            i += 6;
          }
          
        }
      }
    } else {
      resultData[pos] = a;
      pos ++;
      i++;
    }
    
  }
  
}
  
oatpp::String Utils::unescapeString(p_char8 data, v_int32 size, const char* & error, v_int32& errorPosition) {
  
  v_int32 unescapedSize = calcUnescapedStringSize(data, size, error, errorPosition);
  if(error != nullptr){
    return nullptr;
  }
  auto result = String(unescapedSize);
  if(unescapedSize == size) {
    std::memcpy(result->getData(), data, size);
  } else {
    unescapeStringToBuffer(data, size, result->getData());
  }
  return result;
  
}
  
std::string Utils::unescapeStringToStdString(p_char8 data, v_int32 size, const char* & error, v_int32& errorPosition){
  
  v_int32 unescapedSize = calcUnescapedStringSize(data, size, error, errorPosition);
  if(error != nullptr){
    return "";
  }
  std::string result;
  result.resize(unescapedSize);
  if(unescapedSize == size) {
    std::memcpy((p_char8) result.data(), data, size);
  } else {
    unescapeStringToBuffer(data, size, (p_char8) result.data());
  }
  return result;
  
}
  
p_char8 Utils::preparseString(ParsingCaret& caret, v_int32& size){
  
  if(caret.canContinueAtChar('"', 1)){
    
    const p_char8 data = caret.getData();
    v_int32 pos = caret.getPosition();
    v_int32 pos0 = pos;
    v_int32 length = caret.getDataSize();
    
    while (pos < length) {
      v_char8 a = data[pos];
      if(a == '"'){
        size = pos - pos0;
        return &data[pos0];
      } else if(a == '\\') {
        pos += 2;
      } else {
        pos ++;
      }
    }
    caret.setPosition(caret.getDataSize());
    caret.setError("[oatpp::parser::json::Utils::preparseString()]: Error. '\"' - expected", ERROR_CODE_PARSER_QUOTE_EXPECTED);
  } else {
    caret.setError("[oatpp::parser::json::Utils::preparseString()]: Error. '\"' - expected", ERROR_CODE_PARSER_QUOTE_EXPECTED);
  }
  
  return nullptr;
  
}
  
oatpp::String Utils::parseString(ParsingCaret& caret) {
  
  v_int32 size;
  p_char8 data = preparseString(caret, size);
  
  if(data != nullptr) {
  
    v_int32 pos = caret.getPosition();
    
    const char* error;
    v_int32 errorPosition;
    auto result = unescapeString(data, size, error, errorPosition);
    if(error != nullptr){
      caret.setError("[oatpp::parser::json::Utils::parseString()]: Error. Call to unescapeString() failed");
      caret.setPosition(pos + errorPosition);
    } else {
      caret.setPosition(pos + size + 1);
    }
    
    return result;
    
  }
  
  return nullptr;
  
}
  
std::string Utils::parseStringToStdString(ParsingCaret& caret){
  
  v_int32 size;
  p_char8 data = preparseString(caret, size);
  
  if(data != nullptr) {
    
    v_int32 pos = caret.getPosition();
    
    const char* error;
    v_int32 errorPosition;
    const std::string& result = unescapeStringToStdString(data, size, error, errorPosition);
    if(error != nullptr){
      caret.setError("[oatpp::parser::json::Utils::parseStringToStdString()]: Error. Call to unescapeStringToStdString() failed");
      caret.setPosition(pos + errorPosition);
    } else {
      caret.setPosition(pos + size + 1);
    }
    
    return result;
    
  }
  
  return "";
  
}
    
  
}}}
