/*!
  \file json_value_parser-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_JSON_VALUE_PARSER_INL_HPP
#define ZISC_JSON_VALUE_PARSER_INL_HPP

#include "json_value_parser.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <regex>
#include <string_view>
#include <type_traits>
#include <utility>
// Zisc
#include "constant_string.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto JsonValueParser::truePattern() noexcept
{
  auto pattern = toString(R"(true)");
  return pattern;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto JsonValueParser::falsePattern() noexcept
{
  auto pattern = toString(R"(false)");
  return pattern;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto JsonValueParser::booleanPattern() noexcept
{
  auto pattern = truePattern() + R"(|)" + falsePattern();
  return pattern;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto JsonValueParser::integerPattern() noexcept
{
  auto pattern = toString(R"(-?(?:[1-9][0-9]*|0))");
  return pattern;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto JsonValueParser::floatPattern() noexcept
{
  auto pattern = integerPattern() + R"((?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)";
  return pattern;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto JsonValueParser::stringPattern() noexcept
{
  auto character = toString(R"([^"\\[:cntrl:]])");
  auto escaped = toString(R"(\\["\\bfnrt])");
  auto pattern = R"("(?:)" + character + R"(|)" + escaped + R"()*")";
  return pattern;
}

/*!
  \details No detailed description
  */
inline
JsonValueParser::JsonValueParser() noexcept :
    int_pattern_{integerPattern().toCStr(), regexInsOptions()},
    float_pattern_{floatPattern().toCStr(), regexInsOptions()},
    string_pattern_{stringPattern().toCStr(), regexInsOptions()}
{
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::getCxxStringSize(const std::string_view json_value) noexcept -> std::size_t
{
  const std::size_t size = toCxxStringImpl(json_value, nullptr);
  return size;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto JsonValueParser::regexInsOptions() noexcept -> std::regex::flag_type
{
  constexpr std::regex::flag_type flag = std::regex_constants::nosubs |
                                         std::regex_constants::optimize |
                                         std::regex_constants::ECMAScript;
  return flag;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto JsonValueParser::regexTempOptions() noexcept -> std::regex::flag_type
{
  constexpr std::regex::flag_type flag = std::regex_constants::nosubs |
                                         std::regex_constants::ECMAScript;
  return flag;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::toCxxBool(const std::string_view json_value) noexcept -> bool
{
  constexpr auto true_value = truePattern();
  const bool result = true_value == json_value;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] json_value No description.
  \return No description
  */
template <std::floating_point Float> inline
auto JsonValueParser::toCxxFloat(const std::string_view json_value) noexcept -> Float
{
  const Float result = toCxxFloatImpl<Float>(json_value);
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] json_value No description.
  \return No description
  */
template <Integer Int> inline
auto JsonValueParser::toCxxInteger(const std::string_view json_value) noexcept -> Int
{
  const Int result = toCxxIntegerImpl<Int>(json_value);
  return result;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \param [out] value No description.
  */
inline
void JsonValueParser::toCxxString(const std::string_view json_value,
                                  char* value) noexcept
{
  toCxxStringImpl(json_value, value);
}

// For temporary use

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isBoolValue(const std::string_view json_value) noexcept -> bool
{
  constexpr auto true_value = truePattern();
  constexpr auto false_value = falsePattern();
  const bool result = (true_value == json_value) || (false_value == json_value);
  return result;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isFloatValue(const std::string_view json_value) noexcept -> bool
{
  const std::regex float_pattern{floatPattern().toCStr(), regexTempOptions()};
  const bool result = isValueOf(float_pattern, json_value);
  return result;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isIntegerValue(const std::string_view json_value) noexcept -> bool
{
  const std::regex int_pattern{integerPattern().toCStr(), regexTempOptions()};
  const bool result = isValueOf(int_pattern, json_value);
  return result;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isStringValue(const std::string_view json_value) noexcept -> bool
{
  const std::regex string_pattern{stringPattern().toCStr(), regexTempOptions()};
  const bool result = isValueOf(string_pattern, json_value);
  return result;
}

// For instance

/*!
  \details No detailed description

  \return No description
  */
inline
auto JsonValueParser::floatRegex() const noexcept -> const std::regex&
{
  return float_pattern_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto JsonValueParser::integerRegex() const noexcept -> const std::regex&
{
  return int_pattern_;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isBool(const std::string_view json_value) noexcept -> bool
{
  const bool result = isBoolValue(json_value);
  return result;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isFloat(const std::string_view json_value) const noexcept -> bool
{
  const bool result = isValueOf(floatRegex(), json_value);
  return result;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isInteger(const std::string_view json_value) const noexcept -> bool
{
  const bool result = isValueOf(integerRegex(), json_value);
  return result;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isString(const std::string_view json_value) const noexcept -> bool
{
  const bool result = isValueOf(stringRegex(), json_value);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto JsonValueParser::stringRegex() const noexcept -> const std::regex&
{
  return string_pattern_;
}

/*!
  \details No detailed description

  \param [in] c No description.
  \return No description
  */
inline
auto JsonValueParser::getEscapedCharacter(const char c) noexcept -> char
{
  char result = '\0';
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
  \details No detailed description

  \param [in] pattern No description.
  \param [in] json_value No description.
  \return No description
  */
inline
auto JsonValueParser::isValueOf(const std::regex& pattern,
                                const std::string_view json_value) noexcept -> bool
{
  const bool result = std::regex_match(json_value.begin(),
                                       json_value.end(),
                                       pattern);
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] json_value No description.
  \return No description
  */
template <std::floating_point Float> inline
auto JsonValueParser::toCxxFloatImpl(const std::string_view json_value) noexcept -> Float
{
  using FType = std::remove_cv_t<Float>;
  Float result;
  if constexpr (std::is_same_v<float, FType>)
    result = std::strtof(json_value.data(), nullptr);
  else if constexpr (std::is_same_v<double, FType>)
    result = std::strtod(json_value.data(), nullptr);
  else if constexpr (std::is_same_v<long double, FType>)
    result = std::strtold(json_value.data(), nullptr);
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] json_value No description.
  \return No description
  */
template <SignedInteger Int> inline
auto JsonValueParser::toCxxIntegerImpl(const std::string_view json_value) noexcept -> Int
{
  Int result = 0;
  if constexpr (Integer64<Int>)
    result = std::strtoll(json_value.data(), nullptr, 10);
  else
    result = cast<Int>(std::strtol(json_value.data(), nullptr, 10));
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] json_value No description.
  \return No description
  */
template <UnsignedInteger Int> inline
auto JsonValueParser::toCxxIntegerImpl(const std::string_view json_value) noexcept -> Int
{
  Int result = 0;
  if constexpr (Integer64<Int>)
    result = std::strtoull(json_value.data(), nullptr, 10);
  else
    result = cast<Int>(std::strtoul(json_value.data(), nullptr, 10));
  return result;
}

/*!
  \details No detailed description

  \param [in] json_value No description.
  \param [out] value No description.
  \return No description
  */
inline
auto JsonValueParser::toCxxStringImpl(std::string_view json_value,
                                      char* value) noexcept -> std::size_t
{
  // Remove prefix and suffix '"'
  json_value.remove_prefix(1);
  json_value.remove_suffix(1);

  std::size_t size = 0;
  for (auto i = json_value.begin(); i != json_value.end(); ++i) {
    char c = *i;
    if (c == '\\') {
      ++i;
      c = getEscapedCharacter(*i);
    }
    if (value != nullptr)
      value[size] = c;
    ++size;
  }
  return size;
}

} // namespace zisc

#endif // _Z_JSON_VALUE_PARSER_INL_HPP__
