/*!
  \file json_value_parser.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_JSON_VALUE_PARSER_HPP
#define ZISC_JSON_VALUE_PARSER_HPP

// Standard C++ library
#include <cstddef>
#include <regex>
#include <string>
#include <string_view>
// Zisc
#include "string.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief JSON value parser
  \details
  https://www.json.org/index.html
  */
class JsonValueParser
{
  //! JSON value pattern
  struct Pattern
  {
    static constexpr auto getTrue() noexcept
    {
      auto pattern = toString(R"(true)");
      return pattern;
    }

    static constexpr auto getFalse() noexcept
    {
      auto pattern = toString(R"(false)");
      return pattern;
    }

    static constexpr auto getBoolean() noexcept
    {
      auto pattern = Pattern::getTrue() + R"(|)" + Pattern::getFalse();
      return pattern;
    }

    static constexpr auto getInteger() noexcept
    {
      auto pattern = toString(R"(-?(?:[1-9][0-9]*|0))");
      return pattern;
    }

    static constexpr auto getFloat() noexcept
    {
      auto integer = getInteger();
      auto pattern = integer + R"((?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)";
      return pattern;
    }

    static constexpr auto getString() noexcept
    {
      auto character = toString(R"([^"\\[:cntrl:]])");
      auto escaped = toString(R"(\\["\\bfnrt])");
      auto pattern = R"("(?:)" + character + R"(|)" + escaped + R"()*")";
      return pattern;
    }
  };

 public:
  //! Initialize a pattern instance
  JsonValueParser() noexcept;


  //! Return the boolean pattern
  static constexpr auto booleanPattern() noexcept
  {
    return Pattern::getBoolean();
  }

  //! Return the false pattern
  static constexpr auto falsePattern() noexcept
  {
    return Pattern::getFalse();
  }

  //! Return the float pattern
  static constexpr auto floatPattern() noexcept
  {
    return Pattern::getFloat();
  }

  //! Return the floating point regex object
  const std::regex& floatRegex() const noexcept;

  //! Return the integer pattern
  static constexpr auto integerPattern() noexcept
  {
    return Pattern::getInteger();
  }

  //! Return the integer regex object
  const std::regex& integerRegex() const noexcept;

  //! Check whether the 'json_value' is JSON boolean
  bool isBool(const std::string_view json_value) const noexcept;

  //! Check whether the 'json_value' is JSON boolean
  static bool isBoolValue(const std::string_view json_value) noexcept;

  //! Check whether the 'json_value' is JSON float
  bool isFloat(const std::string_view json_value) const noexcept;

  //! Check whether the 'json_value' is JSON float
  static bool isFloatValue(const std::string_view json_value) noexcept;

  //! Check whether the 'json_value' is JSON integer
  bool isInteger(const std::string_view json_value) const noexcept;

  //! Check whether the 'json_value' is JSON integer
  static bool isIntegerValue(const std::string_view json_value) noexcept;

  //! Check whether the 'json_value' is JSON string
  bool isString(const std::string_view json_value) const noexcept;

  //! Check whether the 'json_value' is JSON string
  static bool isStringValue(const std::string_view json_value) noexcept;

  //! Check whether the 'json_value' is JSON type
  template <typename Type>
  bool isType(const std::string_view json_value) const noexcept;

  //! Check whether the 'json_value' is JSON type
  template <typename Type>
  static bool isTypeValue(const std::string_view json_value) noexcept;

  //! Return the string pattern
  static constexpr auto stringPattern() noexcept
  {
    return Pattern::getString();
  }

  //! Return the string regex object
  const std::regex& stringRegex() const noexcept;

  //! Convert JSON boolean to C++ boolean
  bool toCxxBool(const std::string_view json_value) const noexcept;

  //! Convert JSON boolean to C++ boolean
  static bool toCxxBoolValue(const std::string_view json_value) noexcept;

  //! Convert JSON float to C++ float
  template <typename Float>
  Float toCxxFloat(const std::string_view json_value) const noexcept;

  //! Convert JSON float to C++ float
  template <typename Float>
  static Float toCxxFloatValue(const std::string_view json_value) noexcept;

  //! Convert JSON float to C++ float
  template <typename Integer>
  Integer toCxxInteger(const std::string_view json_value) const noexcept;

  //! Convert JSON float to C++ float
  template <typename Integer>
  static Integer toCxxIntegerValue(const std::string_view json_value) noexcept;

  //! Convert JSON string to C++ string
  std::string toCxxString(const std::string_view json_value) const noexcept;

  //! Convert JSON string to C++ string
  static std::string toCxxStringValue(const std::string_view json_value) noexcept;

  //! Convert JSON type to C++ type
  template <typename Type>
  Type toCxxType(const std::string_view json_value) const noexcept;

  template <typename Type>
  static Type toCxxTypeValue(const std::string_view json_value) noexcept;

  //! Return the true pattern
  static constexpr auto truePattern() noexcept
  {
    return Pattern::getTrue();
  }

 private:
  //! Convert JSON float to C++ float
  template <typename Float>
  static Float convertToCxxFloat(std::string_view json_value) noexcept;

  //! Convert JSON integer to C++ integer
  template <typename Integer>
  static Integer convertToCxxInteger(std::string_view json_value) noexcept;

  //! Convert JSON string to C++ string
  static std::string convertToCxxString(std::string_view json_value) noexcept;

  //! Return a escaped character
  static char getEscapedCharacter(const char c) noexcept;

  //! Count the string size as C++ string
  static std::size_t getCxxStringSize(std::string_view json_value) noexcept;

  //! Check whether the 'json_value' is JSON value
  static bool isValue(const std::regex& pattern,
                      const std::string_view json_value) noexcept;

  //! Return the regex options for instance
  static constexpr std::regex::flag_type regexIOptions() noexcept;

  //! Return the regex options for static
  static constexpr std::regex::flag_type regexSOptions() noexcept;


  std::regex int_pattern_;
  std::regex float_pattern_;
  std::regex string_pattern_;
};

} // namespace zisc

#include "json_value_parser-inl.hpp"

#endif // ZISC_JSON_VALUE_PARSER_HPP
