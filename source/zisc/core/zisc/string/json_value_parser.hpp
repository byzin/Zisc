/*!
  \file json_value_parser.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_JSON_VALUE_PARSER_HPP
#define ZISC_JSON_VALUE_PARSER_HPP

// Standard C++ library
#include <concepts>
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
  static auto getCxxStringSize(const std::string_view json_value) noexcept -> std::size_t;

  //! Return the regex options for instance
  static constexpr auto regexInsOptions() noexcept -> std::regex::flag_type;

  //! Return the regex options for temporary use
  static constexpr auto regexTempOptions() noexcept -> std::regex::flag_type;

  //! Convert JSON boolean to C++ boolean
  static auto toCxxBool(const std::string_view json_value) noexcept -> bool;

  //! Convert JSON float to C++ float
  template <std::floating_point Float>
  static auto toCxxFloat(const std::string_view json_value) noexcept -> Float;

  //! Convert JSON float to C++ float
  template <Integer Int>
  static auto toCxxInteger(const std::string_view json_value) noexcept -> Int;

  //! Convert JSON string to C++ string
  static void toCxxString(const std::string_view json_value, char* value) noexcept;

  // For temporary use

  //! Check whether the 'json_value' is JSON boolean
  static auto isBoolValue(const std::string_view json_value) noexcept -> bool;

  //! Check whether the 'json_value' is JSON float
  static auto isFloatValue(const std::string_view json_value) noexcept -> bool;

  //! Check whether the 'json_value' is JSON integer
  static auto isIntegerValue(const std::string_view json_value) noexcept -> bool;

  //! Check whether the 'json_value' is JSON string
  static auto isStringValue(const std::string_view json_value) noexcept -> bool;

  // For instance

  //! Return the floating point regex object
  [[nodiscard]]
  auto floatRegex() const noexcept -> const std::regex&;

  //! Return the integer regex object
  [[nodiscard]]
  auto integerRegex() const noexcept -> const std::regex&;

  //! Check whether the 'json_value' is JSON boolean
  [[nodiscard]]
  static auto isBool(const std::string_view json_value) noexcept -> bool;

  //! Check whether the 'json_value' is JSON float
  [[nodiscard]]
  auto isFloat(const std::string_view json_value) const noexcept -> bool;

  //! Check whether the 'json_value' is JSON integer
  [[nodiscard]]
  auto isInteger(const std::string_view json_value) const noexcept -> bool;

  //! Check whether the 'json_value' is JSON string
  [[nodiscard]]
  auto isString(const std::string_view json_value) const noexcept -> bool;

  //! Return the string regex object
  [[nodiscard]]
  auto stringRegex() const noexcept -> const std::regex&;

 private:
  //! Return a escaped character
  static auto getEscapedCharacter(const char c) noexcept -> char;

  //! Check whether the 'json_value' is JSON value
  static auto isValueOf(const std::regex& pattern,
                        const std::string_view json_value) noexcept -> bool;

  //! Convert JSON float to C++ float
  template <std::floating_point Float>
  static auto toCxxFloatImpl(std::string_view json_value) noexcept -> Float;

  //! Convert JSON integer to C++ integer
  template <SignedInteger Int>
  static auto toCxxIntegerImpl(std::string_view json_value) noexcept -> Int;

  //! Convert JSON integer to C++ integer
  template <UnsignedInteger Int>
  static auto toCxxIntegerImpl(std::string_view json_value) noexcept -> Int;

  //! Convert JSON string to C++ string
  static auto toCxxStringImpl(std::string_view json_value,
                              char* value) noexcept -> std::size_t;


  std::regex int_pattern_;
  std::regex float_pattern_;
  std::regex string_pattern_;
};

} // namespace zisc

#include "json_value_parser-inl.hpp"

#endif // ZISC_JSON_VALUE_PARSER_HPP
