/*!
  \file json_value_parser-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_JSON_VALUE_PARSER_INL_HPP
#define ZISC_JSON_VALUE_PARSER_INL_HPP

#include "json_value_parser.hpp"
// Standard C++ library
#include <cstddef>
#include <cstdlib>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
// Zisc
#include "error.hpp"
#include "utility.hpp"
#include "string.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
inline
JsonValueParser::JsonValueParser() noexcept :
    int_pattern_{integerPattern().toCString(), regexIOptions()},
    float_pattern_{floatPattern().toCString(), regexIOptions()},
    string_pattern_{stringPattern().toCString(), regexIOptions()}
{
}

/*!
  */
inline
const std::regex& JsonValueParser::floatRegex() const noexcept
{
  return float_pattern_;
}

/*!
  */
inline
const std::regex& JsonValueParser::integerRegex() const noexcept
{
  return int_pattern_;
}

/*!
  */
inline
bool JsonValueParser::isBool(const std::string_view json_value) const noexcept
{
  return isBoolValue(json_value);
}

/*!
  */
inline
bool JsonValueParser::isBoolValue(const std::string_view json_value) noexcept
{
  constexpr auto true_value = truePattern();
  constexpr auto false_value = falsePattern();
  const bool result = (true_value == json_value) || (false_value == json_value);
  return result;
}

/*!
  */
inline
bool JsonValueParser::isFloat(const std::string_view json_value) const noexcept
{
  return isValue(floatRegex(), json_value);
}

/*!
  */
inline
bool JsonValueParser::isFloatValue(const std::string_view json_value) noexcept
{
  const std::regex float_pattern{floatPattern().toCString(), regexSOptions()};
  return isValue(float_pattern, json_value);
}


/*!
  */
inline
bool JsonValueParser::isInteger(const std::string_view json_value) const noexcept
{
  return isValue(integerRegex(), json_value);
}

/*!
  */
inline
bool JsonValueParser::isIntegerValue(const std::string_view json_value) noexcept
{
  const std::regex int_pattern{integerPattern().toCString(), regexSOptions()};
  return isValue(int_pattern, json_value);
}

/*!
  */
inline
bool JsonValueParser::isString(const std::string_view json_value) const noexcept
{
  return isValue(stringRegex(), json_value);
}

/*!
  */
inline
bool JsonValueParser::isStringValue(const std::string_view json_value) noexcept
{
  const std::regex string_pattern{stringPattern().toCString(), regexSOptions()};
  return isValue(string_pattern, json_value);
}

/*!
  */
template <typename Type> inline
bool JsonValueParser::isType(const std::string_view json_value) const noexcept
{
  bool result = false;
  if constexpr (kIsBoolean<Type>)
    result = isBool(json_value);
  else if constexpr (kIsInteger<Type>)
    result = isInteger(json_value);
  else if constexpr (kIsFloat<Type>)
    result = isFloat(json_value);
  else if constexpr (kIsStdString<Type>)
    result = isString(json_value);
  return result;
}

/*!
  */
template <typename Type> inline
bool JsonValueParser::isTypeValue(const std::string_view json_value) noexcept
{
  bool result = false;
  if constexpr (kIsBoolean<Type>)
    result = isBoolValue(json_value);
  else if constexpr (kIsInteger<Type>)
    result = isIntegerValue(json_value);
  else if constexpr (kIsFloat<Type>)
    result = isFloatValue(json_value);
  else if constexpr (kIsStdString<Type>)
    result = isStringValue(json_value);
  return result;
}

/*!
  */
inline
const std::regex& JsonValueParser::stringRegex() const noexcept
{
  return string_pattern_;
}

/*!
  */
inline
bool JsonValueParser::toCxxBool(const std::string_view json_value) const noexcept
{
  return toCxxBoolValue(json_value);
}

/*!
  */
inline
bool JsonValueParser::toCxxBoolValue(const std::string_view json_value) noexcept
{
  ZISC_ASSERT(isBoolValue(json_value), "The value isn't JSON boolean.");
  constexpr auto true_value = truePattern();
  const bool result = true_value == json_value;
  return result;
}

/*!
  */
template <typename Float> inline
Float JsonValueParser::toCxxFloat(const std::string_view json_value) const noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isFloat(json_value), "The value isn't JSON float.");
  return convertToCxxFloat<Float>(json_value);
}

/*!
  */
template <typename Float> inline
Float JsonValueParser::toCxxFloatValue(const std::string_view json_value) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isFloatValue(json_value), "The value isn't JSON float.");
  return convertToCxxFloat<Float>(json_value);
}

/*!
  */
template <typename Integer> inline
Integer JsonValueParser::toCxxInteger(const std::string_view json_value)
    const noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  ZISC_ASSERT(isInteger(json_value), "The value isn't JSON integer.");
  return convertToCxxInteger<Integer>(json_value);
}

/*!
  */
template <typename Integer> inline
Integer JsonValueParser::toCxxIntegerValue(const std::string_view json_value)
    noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  ZISC_ASSERT(isIntegerValue(json_value), "The value isn't JSON integer.");
  return convertToCxxInteger<Integer>(json_value);
}

/*!
  */
inline
std::string JsonValueParser::toCxxString(const std::string_view json_value)
    const noexcept
{
  ZISC_ASSERT(isString(json_value), "The value isn't JSON string.");
  return convertToCxxString(json_value);
}

/*!
  */
inline
std::string JsonValueParser::toCxxStringValue(const std::string_view json_value)
    noexcept
{
  ZISC_ASSERT(isStringValue(json_value), "The value isn't JSON string.");
  return convertToCxxString(json_value);
}

/*!
  */
template <typename Type> inline
Type JsonValueParser::toCxxType(const std::string_view json_value) const noexcept
{
  Type result{};
  if constexpr (kIsBoolean<Type>)
    result = toCxxBool(json_value);
  else if constexpr (kIsInteger<Type>)
    result = toCxxInteger<Type>(json_value);
  else if constexpr (kIsFloat<Type>)
    result = toCxxFloat<Type>(json_value);
  else if constexpr (kIsStdString<Type>)
    result = toCxxString(json_value);
  return result;
}

/*!
  */
template <typename Type> inline
Type JsonValueParser::toCxxTypeValue(const std::string_view json_value) noexcept
{
  Type result{};
  if constexpr (kIsBoolean<Type>)
    result = toCxxBoolValue(json_value);
  else if constexpr (kIsInteger<Type>)
    result = toCxxIntegerValue<Type>(json_value);
  else if constexpr (kIsFloat<Type>)
    result = toCxxFloatValue<Type>(json_value);
  else if constexpr (kIsStdString<Type>)
    result = toCxxStringValue(json_value);
  return result;
}

/*!
  */
template <typename Float> inline
Float JsonValueParser::convertToCxxFloat(const std::string_view json_value)
    noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  using FType = std::remove_cv_t<Float>;
  char* end = nullptr;
  Float result;
  if constexpr (std::is_same_v<float, FType>)
    result = cast<FType>(std::strtof(json_value.data(), &end));
  else if constexpr (std::is_same_v<double, FType>)
    result = cast<FType>(std::strtod(json_value.data(), &end));
  else if constexpr (std::is_same_v<long double, FType>)
    result = cast<FType>(std::strtold(json_value.data(), &end));
  return result;
}

/*!
  */
template <typename Integer> inline
Integer JsonValueParser::convertToCxxInteger(const std::string_view json_value)
    noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  char* end = nullptr;
  const auto result = std::strtoll(json_value.data(), &end, 10);
  return cast<Integer>(result);
}

/*!
  */
inline
std::string JsonValueParser::convertToCxxString(std::string_view json_value)
    noexcept
{
  // Get C++ string size
  const std::size_t size = getCxxStringSize(json_value);
  std::string result;
  result.reserve(size);

  // Remove the prefix and suffix '"'
  json_value.remove_prefix(1);
  json_value.remove_suffix(1);

  for (auto i = json_value.begin(); i != json_value.end(); ++i) {
    char c = *i;
    if (c == '\\') {
      ++i;
      c = getEscapedCharacter(*i);
    }
    result.push_back(c);
  }

  return result;
}

/*!
  */
inline
char JsonValueParser::getEscapedCharacter(const char c) noexcept
{
  char result = '\\';
  switch (c) {
   case '\"':
    result = '\"';
    break;
   case '\\':
    result = '\\';
    break;
   case 'b':
    result = '\b';
    break;
   case 'f':
    result = '\f';
    break;
   case 'n':
    result = '\n';
    break;
   case 'r':
    result = '\r';
    break;
   case 't':
    result = '\t';
    break;
   default:
    break;
  }
  return result;
}

/*!
  */
inline
std::size_t JsonValueParser::getCxxStringSize(std::string_view json_value)
    noexcept
{
  // Remove the prefix and suffix '"'
  json_value.remove_prefix(1);
  json_value.remove_suffix(1);

  std::size_t size = 0;
  for (auto i = json_value.begin(); i != json_value.end(); ++i, ++size) {
    if ((*i) == '\\')
      ++i;
  }
  return size;
}

/*!
  */
inline
bool JsonValueParser::isValue(const std::regex& pattern,
                              const std::string_view json_value) noexcept
{
  return std::regex_match(json_value.begin(), json_value.end(), pattern);
}

/*!
  */
inline
constexpr auto JsonValueParser::regexIOptions() noexcept -> std::regex::flag_type
{
  constexpr std::regex::flag_type flag =
      std::regex_constants::nosubs |
      std::regex_constants::optimize |
      std::regex_constants::ECMAScript;
  return flag;
}

/*!
  */
inline
constexpr auto JsonValueParser::regexSOptions() noexcept -> std::regex::flag_type
{
  constexpr std::regex::flag_type flag =
      std::regex_constants::nosubs |
      std::regex_constants::ECMAScript;
  return flag;
}

} // namespace zisc

#endif // _Z_JSON_VALUE_PARSER_INL_HPP__
