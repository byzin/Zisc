/*!
  \file json_value_parser.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_JSON_VALUE_PARSER_HPP
#define ZISC_JSON_VALUE_PARSER_HPP

// Standard C++ library
#include <cstddef>
#include <regex>
#include <string_view>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief JSON value parser

  For more detail, please see the following link:
  <a href="https://www.json.org/json-en.html">Introducing JSON</a>.
  */
class JsonValueParser
{
 public:
  //! Initialize a pattern instance
  JsonValueParser() noexcept;


  // Patterns

  //! Return the true string pattern
  static constexpr auto truePattern() noexcept;

  //! Return the false string pattern
  static constexpr auto falsePattern() noexcept;

  //! Return the boolean string pattern
  static constexpr auto booleanPattern() noexcept;

  //! Return the integer string pattern
  static constexpr auto integerPattern() noexcept;

  //! Return the float string pattern
  static constexpr auto floatPattern() noexcept;

  //! Return the string pattern
  static constexpr auto stringPattern() noexcept;


  //! Return the required size for converting the given string to a C++ string
  static std::size_t getCxxStringSize(const std::string_view json_value) noexcept;

  //! Return the regex options for instance
  static constexpr std::regex::flag_type regexInsOptions() noexcept;

  //! Return the regex options for temporary use
  static constexpr std::regex::flag_type regexTempOptions() noexcept;

  //! Convert JSON boolean to C++ boolean
  static bool toCxxBool(const std::string_view json_value) noexcept;

  //! Convert JSON float to C++ float
  template <FloatingPoint Float>
  static Float toCxxFloat(const std::string_view json_value) noexcept;

  //! Convert JSON float to C++ float
  template <Integer Int>
  static Int toCxxInteger(const std::string_view json_value) noexcept;

  //! Convert JSON string to C++ string
  static void toCxxString(const std::string_view json_value, char* value) noexcept;

  // For temporary use

  //! Check whether the 'json_value' is JSON boolean
  static bool isBoolValue(const std::string_view json_value) noexcept;

  //! Check whether the 'json_value' is JSON float
  static bool isFloatValue(const std::string_view json_value) noexcept;

  //! Check whether the 'json_value' is JSON integer
  static bool isIntegerValue(const std::string_view json_value) noexcept;

  //! Check whether the 'json_value' is JSON string
  static bool isStringValue(const std::string_view json_value) noexcept;

  // For instance

  //! Return the floating point regex object
  const std::regex& floatRegex() const noexcept;

  //! Return the integer regex object
  const std::regex& integerRegex() const noexcept;

  //! Check whether the 'json_value' is JSON boolean
  bool isBool(const std::string_view json_value) const noexcept;

  //! Check whether the 'json_value' is JSON float
  bool isFloat(const std::string_view json_value) const noexcept;

  //! Check whether the 'json_value' is JSON integer
  bool isInteger(const std::string_view json_value) const noexcept;

  //! Check whether the 'json_value' is JSON string
  bool isString(const std::string_view json_value) const noexcept;

  //! Return the string regex object
  const std::regex& stringRegex() const noexcept;

 private:
  //! Return a escaped character
  static char getEscapedCharacter(const char c) noexcept;

  //! Check whether the 'json_value' is JSON value
  static bool isValueOf(const std::regex& pattern,
                        const std::string_view json_value) noexcept;

  //! Convert JSON float to C++ float
  template <FloatingPoint Float>
  static Float toCxxFloatImpl(std::string_view json_value) noexcept;

  //! Convert JSON integer to C++ integer
  template <SignedInteger Int>
  static Int toCxxIntegerImpl(std::string_view json_value) noexcept;

  //! Convert JSON integer to C++ integer
  template <UnsignedInteger Int>
  static Int toCxxIntegerImpl(std::string_view json_value) noexcept;

  //! Convert JSON string to C++ string
  static std::size_t toCxxStringImpl(std::string_view json_value,
                                     char* value) noexcept;


  std::regex int_pattern_;
  std::regex float_pattern_;
  std::regex string_pattern_;
};

} // namespace zisc

#include "json_value_parser-inl.hpp"

#endif // ZISC_JSON_VALUE_PARSER_HPP
