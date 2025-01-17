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

#ifndef oatpp_parser_Caret_hpp
#define oatpp_parser_Caret_hpp

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace parser {

/**
 * Helper class to do parsing operations
 */
class Caret {
public:
  static const char* const ERROR_INVALID_INTEGER;
  static const char* const ERROR_INVALID_FLOAT;
  static const char* const ERROR_INVALID_BOOLEAN;
  static const char* const ERROR_NO_OPEN_TAG;
  static const char* const ERROR_NO_CLOSE_TAG;
  static const char* const ERROR_NAME_EXPECTED;
public:

  class Label {
  private:
    Caret* m_caret;
    v_int32 m_start;
    v_int32 m_end;
  public:
    
    Label(Caret* caret);
    
    void start();
    void end();
    p_char8 getData();
    v_int32 getSize();
    oatpp::String toString(bool saveAsOwnData);
    oatpp::String toString();
    std::string std_str();

    explicit operator bool() const;
    
  };

  class StateSaveGuard {
  private:
    Caret& m_caret;
    v_int32 m_savedPosition;
    const char* m_savedErrorMessage;
    v_int32 m_savedErrorCode;
  public:

    StateSaveGuard(Caret& caret);
    ~StateSaveGuard();

    v_int32 getSavedPosition();
    const char* getSavedErrorMessage();
    v_int32 getSavedErrorCode();

  };

private:
  p_char8 m_data;
  v_int32 m_size;
  v_int32 m_pos;
  const char* m_errorMessage;
  v_int32 m_errorCode;
public:
  Caret(const char* text);
  Caret(p_char8 parseData, v_int32 dataSize);
  Caret(const oatpp::String& str);
public:
  
  static std::shared_ptr<Caret> createShared(const char* text);
  static std::shared_ptr<Caret> createShared(p_char8 parseData, v_int32 dataSize);
  static std::shared_ptr<Caret> createShared(const oatpp::String& str);

  virtual ~Caret();

  /**
   * Get pointer to a data, passed to Caret constructor
   * @return
   */
  p_char8 getData();

  /**
   * Same as &getData()[position]
   * @return
   */
  p_char8 getCurrData();

  /**
   * Get size of a data
   * @return
   */
  v_int32 getDataSize();

  /**
   * Set caret position relative to data
   * @param position
   */
  void setPosition(v_int32 position);

  /**
   * Get caret position relative to data
   * @return
   */
  v_int32 getPosition();

  /**
   * Set error message and error code.
   * Note that once error message is set, methods canContinue... will return false
   * @param errorMessage
   * @param errorCode
   */
  void setError(const char* errorMessage, v_int32 errorCode = 0);

  /**
   * Get error message
   * @return error message
   */
  const char* getErrorMessage();

  /**
   * Get error code
   * @return error code
   */
  v_int32 getErrorCode();

  /**
   * Check if error is set for the Caret
   * @return
   */
  bool hasError();

  /**
   * Clear error message and error code
   */
  void clearError();

  /**
   * Create Label(this);
   * @return Label
   */
  Label putLabel();

  /**
   * Increase caret position by one
   */
  void inc();

  /**
   * Increase caret position by amount
   * @param amount
   */
  void inc(v_int32 amount);

  /**
   * Skip chars: [' ', '\t', '\n', '\r','\f']
   * @return true if other char found
   */
  bool skipBlankChars();

  /**
   * Skip char
   * @param c
   * @return true if other char found
   */
  bool skipChar(v_char8 c);

  /**
   * Find char. Position will be set to a found char. If
   * no such char found - position will be set to a dataSize;
   * @param c
   * @return true if found
   */
  bool findChar(v_char8 c);

  /**
   * Skip chars defined by set.
   * ex. skipCharsFromSet("abc") - will skip all 'a', 'b', 'c' chars
   * @param set
   * @return true if other char found
   */
  bool skipCharsFromSet(const char* set);

  /**
   * Skip chars defined by set.
   * ex. skipCharsFromSet("abc", 3) - will skip all 'a', 'b', 'c' chars
   * @param set
   * @param setSize
   * @return true if other char found
   */
  bool skipCharsFromSet(p_char8 set, v_int32 setSize);

  /**
   * Find one of chars defined by set.
   * @param set
   * @return char found or -1 if no char found
   */
  v_int32 findCharFromSet(const char* set);

  /**
   * Find one of chars defined by set.
   * @param set
   * @param setSize
   * @return char found or -1 if no char found
   */
  v_int32 findCharFromSet(p_char8 set, v_int32 setSize);

  /**
   * Find "\r\n" chars
   * @return true if found
   */
  bool findRN();

  /**
   * Skip "\r\n"
   * @return True if position changes. False if caret not at "\r\n"
   */
  bool skipRN();

  /**
   * Check if caret at "\r\n" chars
   * @return
   */
  bool isAtRN();

  /**
   * Find '\r' char of '\n' char
   * @return true if found '\r' or '\n'
   */
  bool findROrN();

  /**
   * if at "\r\n" - skip.
   * if at "\n" - skip.
   * @return true if position changed
   */
  bool skipRNOrN();

  /**
   * skip any sequence of '\r' and '\n'
   * @return true if position changed
   */
  bool skipAllRsAndNs();

  /**
   * parse integer value starting from the current position.
   * Using function std::strtol()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @param base - base is passed to std::strtol function
   * @return parsed value
   */
  long int parseInt(int base = 10);

  /**
   * parse integer value starting from the current position.
   * Using function std::strtoul()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @param base - base is passed to std::strtoul function
   * @return parsed value
   */
  unsigned long int parseUnsignedInt(int base = 10);

  /**
   * parse float value starting from the current position.
   * Using function std::strtof()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @return parsed value
   */
  v_float32 parseFloat32();

  /**
   * parse float value starting from the current position.
   * Using function std::strtod()
   *
   * Warning: position may go out of @Caret::getSize() bound.
   *
   * @return parsed value
   */
  v_float64 parseFloat64();

  /**
   * Check if follows text
   * @param text
   * @param skipIfTrue - increase position if true
   * @return
   */
  bool isAtText(const char* text, bool skipIfTrue = false);

  /**
   * Check if follows text
   * @param text
   * @param textSize
   * @param skipIfTrue - increase position if true
   * @return
   */
  bool isAtText(p_char8 text, v_int32 textSize, bool skipIfTrue = false);

  /**
   * Check if follows text (Not Case Sensitive)
   * @param text
   * @param skipIfTrue - increase position if true
   * @return
   */
  bool isAtTextNCS(const char* text, bool skipIfTrue = false);

  /**
   * Check if follows text (Not Case Sensitive)
   * @param text
   * @param textSize
   * @param skipIfTrue - increase position if true
   * @return
   */
  bool isAtTextNCS(p_char8 text, v_int32 textSize, bool skipIfTrue = false);

  /**
   * Parse enclosed string.
   * ex. for data "'let\'s go'" parseStringEnclosed('\'', '\'', '\\')
   * will return Label to "let\'s go" without enclosing '\'' chars
   * @param openChar
   * @param closeChar
   * @param escapeChar
   * @return
   */
  Label parseStringEnclosed(char openChar, char closeChar, char escapeChar);

  /**
   * Find text and set position to found text
   * @param text
   * @return true if found
   */
  bool findText(const char* text);

  /**
   * Find text and set position to found text
   * @param text
   * @param textSize
   * @return true if found
   */
  bool findText(p_char8 text, v_int32 textSize);

  /**
   * Check if caret is at char defined by set
   * ex. isAtCharFromSet("abc") - will return true for 'a', 'b', 'c' chars
   * @param set
   * @return
   */
  bool isAtCharFromSet(const char* set) const;

  /**
   * Check if caret is at char defined by set
   * ex. isAtCharFromSet("abc", 3) - will return true for 'a', 'b', 'c' chars
   * @param set
   * @param setSize
   * @return
   */
  bool isAtCharFromSet(p_char8 set, v_int32 setSize) const;

  /**
   * Check if caret is at char
   * @param c
   * @return
   */
  bool isAtChar(v_char8 c) const;

  /**
   * Check if caret is at one of chars [' ', '\t', '\n', '\r','\f']
   * @return
   */
  bool isAtBlankChar() const;

  /**
   * Check if caret is at digit
   * @return
   */
  bool isAtDigitChar() const;

  /**
   * Check if caret is at char, and no error is set
   * @param c
   * @return
   */
  bool canContinueAtChar(v_char8 c) const;

  /**
   * Check if caret is at char, and no error is set.
   * If true inc position by skipChars
   * @param c
   * @param skipChars
   * @return
   */
  bool canContinueAtChar(v_char8 c, v_int32 skipChars);

  /**
   * Check if caret position < dataSize and not error is set
   * @return
   */
  bool canContinue() const;

};
  
}}



#endif /* oatpp_parser_Caret_hpp */
