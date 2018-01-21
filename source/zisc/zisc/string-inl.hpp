/*!
  \file string-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STRING_INL_HPP
#define ZISC_STRING_INL_HPP

#include "string.hpp"
// Standard C++ library
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <regex>
#include <string>
#include <utility>
// Zisc
#include "error.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr BasicString<CharType, N>::BasicString() noexcept
    : string_{'\0'}
{
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr BasicString<CharType, N>::BasicString(const CharType (&string)[N]) noexcept
    : string_{'\0'}
{
  initialize(string);
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr bool BasicString<CharType, N>::operator==(
    const CharType* other) const noexcept
{
  uint m = 0;
  for (; other[m] != '\0'; ++m) {}
  ++m;

  bool flag = (N == m);
  for (uint i = 0; flag && (i < N); ++i)
    flag = (get(i) == other[i]);
  return flag;
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> template <uint M> inline
constexpr bool BasicString<CharType, N>::operator==(
    const BasicString<CharType, M>& other) const noexcept
{
  bool flag = (N == M);
  for (uint i = 0; flag && (i < N); ++i)
    flag = (get(i) == other[i]);
  return flag;
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr bool BasicString<CharType, N>::operator!=(
    const CharType* other) const noexcept
{
  return !(*this == other);
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> template <uint M> inline
constexpr bool BasicString<CharType, N>::operator!=(
    const BasicString<CharType, M>& other) const noexcept
{
  return !(*this == other);
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr CharType& BasicString<CharType, N>::operator[](
    const uint index) noexcept
{
  return get(index);
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr const CharType& BasicString<CharType, N>::operator[](
    const uint index) const noexcept
{
  return get(index);
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr CharType& BasicString<CharType, N>::get(
    const uint index) noexcept
{
  return string_[index];
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr const CharType& BasicString<CharType, N>::get(
    const uint index) const noexcept
{
  return string_[index];
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr uint BasicString<CharType, N>::size() const noexcept
{
  return N - 1;
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
const CharType* BasicString<CharType, N>::toCString() const noexcept
{
  return string_;
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
std::string BasicString<CharType, N>::toStdString() const noexcept
{
  return std::string{string_};
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr void BasicString<CharType, N>::initialize(
    const CharType (&string)[N]) noexcept
{
  for (uint i = 0; i < size(); ++i)
    string_[i] = string[i];
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N> inline
constexpr void BasicString<CharType, N>::initialize(
    const BasicString& other) noexcept
{
  for (uint i = 0; i < size(); ++i)
    string_[i] = other[i];
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N1, uint N2> inline
constexpr BasicString<CharType, N1 + N2 - 1> operator+(
    const BasicString<CharType, N1>& string1,
    const BasicString<CharType, N2>& string2) noexcept
{
  return concatenate(string1, string2);
}

/*!
  \details
  No detailed.
  */
template <uint N1, uint N2> inline
constexpr String<N1 + N2 - 1> operator+(
    const char (&string1)[N1],
    const String<N2>& string2) noexcept
{
  return concatenate(string1, string2);
}

/*!
  \details
  No detailed.
  */
template <uint N1, uint N2> inline
constexpr String<N1 + N2 - 1> operator+(
    const String<N1>& string1,
    const char (&string2)[N2]) noexcept
{
  return concatenate(string1, string2);
}

/*!
  \details
  No detailed.
  */
template <typename CharType, uint N1, uint N2> inline
constexpr BasicString<CharType, N1 + N2 - 1> concatenate(
    const BasicString<CharType, N1>& string1,
    const BasicString<CharType, N2>& string2) noexcept
{
  constexpr uint size = N1 + N2 - 1;
  BasicString<CharType, size> string;
  for (uint i = 0; i < string1.size(); ++i)
    string[i] = string1[i];
  for (uint i = 0; i < string2.size(); ++i)
    string[i + string1.size()] = string2[i];
  string[size - 1] = '\0';
  return string;
}

/*!
  \details
  No detailed.
  */
template <uint N1, uint N2> inline
constexpr String<N1 + N2 - 1> concatenate(
    const char (&string1)[N1],
    const String<N2>& string2) noexcept
{
  auto str1 = toString(string1);
  auto result = concatenate(str1, string2);
  return result;
}

/*!
  \details
  No detailed.
  */
template <uint N1, uint N2> inline
constexpr String<N1 + N2 - 1> concatenate(
    const String<N1>& string1,
    const char (&string2)[N2]) noexcept 
{
  auto str2 = toString(string2);
  auto result = concatenate(string1, str2);
  return result;
}

/*!
  \details
  No detailed.
  */
template <uint N> inline
constexpr String<N> toString(const char (&string)[N]) noexcept
{
  return String<N>{string};
}

/*!
  */
inline
constexpr std::size_t getStringLength(const char* string) noexcept
{
  std::size_t n = 0;
  for (; string[n] != '\0'; ++n) {}
  return n;
}

namespace inner {

/*!
  \details
  No detailed.
  */
class ValueStringPattern
{
 public:
  static constexpr auto boolean() noexcept
  {
    auto boolean = toString("true|false");
    return boolean;
  }

  static constexpr auto integer() noexcept
  {
    //auto integer = "-?(?:" + digit + "|[1-9]" + digits + ")";
    auto integer = toString(R"(-?(?:0|[1-9]\d*))");
    return integer;
  }

  static constexpr auto floatingPoint() noexcept
  {
    // sub-patterns
    auto digit = toString(R"(\d)");
    auto digits = digit + "+";
    auto frac = R"(\.)" + digits;
    auto e = toString(R"(e|E|e\+|E\+|e-|E-)");
    auto exp = "(?:" + e + ")" + digits;
    auto i = integer();
    // pattern
    auto floating_point = i + "(?:" + frac + ")?" + "(?:" + exp + ")?";
    return floating_point;
  }

  static constexpr auto string() noexcept
  {
    auto character = toString(R"([^"\\[:cntrl:]])");
    auto escaped = toString(R"(\\["\\/bfnrt])");
    auto string = R"("(?:)" + character + "|" + escaped + R"()*")";
    return string;
  }
};

} // namespace inner

/*!
  \details
  No detailed.
  */
inline
ValueString::ValueString() noexcept :
    float_pattern_{inner::ValueStringPattern::floatingPoint().toCString(),
                   regexOption()},
    int_pattern_{inner::ValueStringPattern::integer().toCString(),
                 regexOption()},
    string_pattern_{inner::ValueStringPattern::string().toCString(),
                    regexOption()}
{
}

/*!
  \details
  No detailed.
  */
inline
bool ValueString::isBoolean(const char* string) const noexcept
{
  return isBooleanString(string);
}

/*!
  \details
  No detailed.
  */
inline
bool ValueString::isBoolean(const std::string& string) const noexcept
{
  return isBooleanString(string);
}

/*!
  \details
  No detailed.
  */
inline
bool ValueString::isFloat(const char* string) const noexcept
{
  return std::regex_match(string, float_pattern_);
}

/*!
  \details
  No detailed.
  */
inline
bool ValueString::isFloat(const std::string& string) const noexcept
{
  return std::regex_match(string, float_pattern_);
}

/*!
  \details
  No detailed.
  */
inline
bool ValueString::isInteger(const char* string) const noexcept
{
  return std::regex_match(string, int_pattern_);
}

/*!
  \details
  No detailed.
  */
inline
bool ValueString::isInteger(const std::string& string) const noexcept
{
  return std::regex_match(string, int_pattern_);
}

/*!
  \details
  No detailed.
  */
inline
bool ValueString::isCxxString(const char* string) const noexcept
{
  return std::regex_match(string, string_pattern_);
}

/*!
  \details
  No detailed.
  */
inline
bool ValueString::isCxxString(const std::string& string) const noexcept
{
  return std::regex_match(string, string_pattern_);
}

/*!
  \details
  No detailed.
  */
inline
constexpr auto ValueString::regexOption() noexcept -> RegexOptionType
{
  return std::regex_constants::nosubs |
         std::regex_constants::optimize |
         std::regex_constants::ECMAScript;
}

/*!
 \details
 No detailed.
 */
inline
bool toBoolean(const char* string) noexcept
{
  return (toString("true") == string);
}

/*!
 \details
 No detailed.
 */
inline
bool toBoolean(const std::string& string) noexcept
{
  return (toString("true") == string.c_str());
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float toFloat(const char* string) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  return cast<Float>(std::atof(string));
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float toFloat(const std::string& string) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  return cast<Float>(std::stod(string));
}

/*!
  \details
  No detailed.
  */
template <> inline
float toFloat(const std::string& string) noexcept
{
  return std::stof(string);
}

/*!
  \details
  No detailed.
  */
template <> inline
long double toFloat(const std::string& string) noexcept
{
  return std::stold(string);
}

/*!
  \details
  No detailed.
  */
template <typename Integer> inline
Integer toInteger(const char* string) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  return cast<Integer>(std::atoi(string));
}

/*!
  \details
  No detailed.
  */
template <> inline
long toInteger(const char* string) noexcept
{
  return std::atol(string);
}

/*!
  \details
  No detailed.
  */
template <> inline
long long toInteger(const char* string) noexcept
{
  return std::atoll(string);
}

/*!
  \details
  No detailed.
  */
template <typename Integer> inline
Integer toInteger(const std::string& string) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  return cast<Integer>(std::stoi(string));
}

/*!
  \details
  No detailed.
  */
template <> inline
long toInteger(const std::string& string) noexcept
{
  return std::stol(string);
}

/*!
  \details
  No detailed.
  */
template <> inline
long long toInteger(const std::string& string) noexcept
{
  return std::stoll(string);
}

/*!
  \details
  No detailed.
  */
template <> inline
unsigned int toInteger(const std::string& string) noexcept
{
  return cast<unsigned int>(std::stoul(string));
}

/*!
  \details
  No detailed.
  */
template <> inline
unsigned long toInteger(const std::string& string) noexcept
{
  return std::stoul(string);
}

/*!
  \details
  No detailed.
  */
template <> inline
unsigned long long toInteger(const std::string& string) noexcept
{
  return std::stoull(string);
}

/*!
  \details
  No detailed.
  */
inline
std::string toCxxString(const char* string) noexcept
{
  return toCxxString(std::string{string});
}

/*!
  \details
  No detailed.
  */
inline
std::string toCxxString(const std::string& string) noexcept
{
  return string.substr(1, string.size() - 2);
}

namespace inner {

template <typename Type, typename String> inline
Type toCxxValue(const String& string, EnableIfBoolean<Type> = kEnabler) noexcept
{
  return toBoolean(string);
}

template <typename Type, typename String> inline
Type toCxxValue(const String& string, EnableIfFloat<Type> = kEnabler) noexcept
{
  return toFloat<Type>(string);
}

template <typename Type, typename String> inline
Type toCxxValue(const String& string, EnableIfInteger<Type> = kEnabler) noexcept
{
  return toInteger<Type>(string);
}

template <typename Type, typename String> inline
Type toCxxValue(const String& string, EnableIfSame<std::string, Type> = kEnabler) noexcept
{
  return toCxxString(string);
}

} // namespace inner

/*!
  \details
  No detailed.
  */
template <typename Type> inline
Type toCxxValue(const char* string) noexcept
{
  return inner::toCxxValue<const char*, Type>(string);
}

/*!
  \details
  No detailed.
  */
template <typename Type> inline
Type toCxxValue(const std::string& string) noexcept
{
  return inner::toCxxValue<std::string, Type>(string);
}

namespace inner {

/*!
  \details
  No detailed.
  */
template <typename Type> inline
bool isFloatString(const Type& string) noexcept
{
  constexpr auto regex_option =  std::regex_constants::nosubs | 
                                 std::regex_constants::ECMAScript;
  constexpr auto float_string = ValueStringPattern::floatingPoint();
  std::regex float_pattern{float_string.toCString(), regex_option};
  return std::regex_match(string, float_pattern);
}

/*!
  \details
  No detailed.
  */
template <typename Type> inline
bool isIntegerString(const Type& string) noexcept
{
  constexpr auto regex_option =  std::regex_constants::nosubs | 
                                 std::regex_constants::ECMAScript;
  constexpr auto integer_string = ValueStringPattern::integer();
  std::regex int_pattern{integer_string.toCString(), regex_option};
  return std::regex_match(string, int_pattern);
}

/*!
  \details
  No detailed.
  */
template <typename Type> inline
bool isCxxStringString(const Type& string) noexcept
{
  constexpr auto regex_option =  std::regex_constants::nosubs | 
                                 std::regex_constants::ECMAScript;
  constexpr auto string_string = ValueStringPattern::string();
  std::regex string_pattern{string_string.toCString(), regex_option};
  return std::regex_match(string, string_pattern);
}

} // namespace inner

/*!
 \details
 No detailed.
 */
inline
bool isBooleanString(const char* string) noexcept
{
  return (toString("true") == string) || 
         (toString("false") == string);
}

/*!
 \details
 No detailed.
 */
inline
bool isBooleanString(const std::string& string) noexcept
{
  return (toString("true") == string.c_str()) || 
         (toString("false") == string.c_str());
}

/*!
 \details
 No detailed.
 */
inline
bool isFloatString(const char* string) noexcept
{
  return inner::isFloatString(string);
}

/*!
 \details
 No detailed.
 */
inline
bool isFloatString(const std::string& string) noexcept
{
  return inner::isFloatString(string);
}

/*!
 \details
 No detailed.
 */
inline
bool isIntegerString(const char* string) noexcept
{
  return inner::isIntegerString(string);
}

/*!
 \details
 No detailed.
 */
inline
bool isIntegerString(const std::string& string) noexcept
{
  return inner::isIntegerString(string);
}

/*!
 \details
 No detailed.
 */
inline
bool isCxxStringString(const char* string) noexcept
{
  return inner::isCxxStringString(string);
}

/*!
 \details
 No detailed.
 */
inline
bool isCxxStringString(const std::string& string) noexcept
{
  return inner::isCxxStringString(string);
}

/*!
  \details
  No detailed.
  */
inline
std::string& trim(std::string* string) noexcept
{
  using std::find_if;

  auto& str = *string;
  // Erase head spaces
  auto head = 
      find_if(str.begin(), str.end(), [](char& ch) {return !isspace(ch);});
  str.erase(str.begin(), head);
  // Erase tail spaces
  auto tail = 
      find_if(str.rbegin(), str.rend(), [](char& ch) {return !isspace(ch);}).base();
  str.erase(tail, str.end());
  return str;
}

/*!
  \details
  No detailed.
  */
inline
std::string trimmed(const std::string& string) noexcept
{
  auto str = string;
  trim(&str);
  return str;
}

/*!
  */
inline
void replaceLineFeedCodeToUnixStyle(std::string* text) noexcept
{
  ZISC_ASSERT(text != nullptr, "The text is NULL.");
  constexpr char lf = '\n';
  std::string::size_type position = 0;
  // Replace CR + LF to LF
  constexpr char crlf[] = "\r\n";
  position = text->find(crlf, position);
  while (position != std::string::npos) {
    (*text)[position] = ' ';
    (*text)[position + 1] = lf;
    position = text->find(crlf, position);
  }
  // Replace CR to LF
  constexpr char cr = '\r';
  for (position = 0; position < text->size(); ++position) {
    if ((*text)[position] == cr)
      (*text)[position] = lf;
  }
}

/*!
  */
inline
void replaceLineFeedCodeToSpaces(std::string* text) noexcept
{
  ZISC_ASSERT(text != nullptr, "The text is NULL.");
  constexpr char cr = '\r';
  constexpr char lf = '\n';
  for (std::string::size_type i = 0; i < (text->size()); ++i) {
    auto& c = (*text)[i];
    if ((c == cr) || (c == lf))
      c = ' ';
  }
}

} // namespace zisc

#endif // _Z_STRING_INL_HPP__
