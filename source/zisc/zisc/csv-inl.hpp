/*!
  \file csv-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CSV_INL_HPP
#define ZISC_CSV_INL_HPP

#include "csv.hpp"
// Standard C++ library
#include <istream>
#include <list>
#include <regex>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
// Zisc
#include "error.hpp"
#include "json_value_parser.hpp"
#include "memory_resource.hpp"
#include "string.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

namespace inner {

template <typename Type> inline
constexpr auto getTypePattern(EnableIfBoolean<Type> = kEnabler) noexcept
{
  return JsonValueParser::booleanPattern();
}

template <typename Type> inline
constexpr auto getTypePattern(EnableIfFloat<Type> = kEnabler) noexcept
{
  return JsonValueParser::floatPattern();
}

template <typename Type> inline
constexpr auto getTypePattern(EnableIfInteger<Type> = kEnabler) noexcept
{
  return JsonValueParser::integerPattern();
}

template <typename Type> inline
constexpr auto getTypePattern(EnableIfSame<std::string, Type> = kEnabler) noexcept
{
  return JsonValueParser::stringPattern();
}

template <typename Type> inline
constexpr auto getCsvPattern() noexcept
{
  return R"(\s*()" + getTypePattern<Type>() + R"()\s*)";
}

template <typename Type1, typename Type2, typename ...Types> inline
constexpr auto getCsvPattern() noexcept
{
  return getCsvPattern<Type1>() + R"(,)" + getCsvPattern<Type2, Types...>();
}

} // namespace inner 

// public member function

/*!
  \details
  No detailed.
  */
template <typename Type, typename ...Types> inline
Csv<Type, Types...>::Csv(pmr::memory_resource* mem_resource) noexcept : 
    data_{pmr::polymorphic_allocator<RecordType>{mem_resource}},
    csv_pattern_{inner::getCsvPattern<Type, Types...>().toCString(), 
                 std::regex_constants::optimize | std::regex_constants::ECMAScript}
{
}

/*!
 \details
 No detailed.
 */
template <typename Type, typename ...Types> inline
void Csv<Type, Types...>::append(const std::string& csv,
                                 std::list<std::string>* message_list) noexcept
{
  std::istringstream csv_text{csv};
  append(csv_text, message_list);
}

/*!
 \details
 No detailed.
 */
template <typename Type, typename ...Types> inline
void Csv<Type, Types...>::append(std::istream& csv,
                                 std::list<std::string>* message_list) noexcept
{
  for (std::string line; std::getline(csv, line);) {
    auto record = parseCsvLine(line, message_list);
    data_.emplace_back(std::move(record));
  }
}

/*!
  \details
  No detailed.
  */
template <typename Type, typename ...Types> inline
void Csv<Type, Types...>::clear() noexcept
{
  data_.clear();
}

/*!
  */
template <typename Type, typename ...Types> inline
std::string_view Csv<Type, Types...>::csvPattern() noexcept
{
  constexpr auto pattern = inner::getCsvPattern<Type, Types...>();
  return std::string_view{pattern.toCString(), pattern.size()};
}

/*!
  */
template <typename Type, typename ...Types> inline
const std::regex& Csv<Type, Types...>::csvRegex() const noexcept
{
  return csv_pattern_;
}

/*!
  \details
  No detailed.
  */
template <typename Type, typename ...Types> inline
constexpr uint Csv<Type, Types...>::columnSize() noexcept
{
  constexpr uint size = 1 + sizeof...(Types);
  return size;
}

/*!
 \details
 No detailed.
 */
template <typename Type, typename ...Types> inline
auto Csv<Type, Types...>::record(const uint row) const noexcept -> const RecordType&
{
  ZISC_ASSERT(row < rowSize(), "The row is out of range.");
  auto record = data_.begin();
  std::advance(record, row);
  return *record;
}

/*!
 \details
 No detailed.
 */
template <typename Type, typename ...Types> inline
uint Csv<Type, Types...>::rowSize() const noexcept
{
  return cast<uint>(data_.size());
}

/*!
 \details
 No detailed.
 */
template <typename Type, typename ...Types> template <uint column> inline
auto Csv<Type, Types...>::get(const uint row) const noexcept
    -> const FieldType<column>&
{
  const auto& record = this->record(row);
  return std::get<column>(record);
}

/*!
  */
template <typename Type, typename ...Types> template <std::size_t index> inline
auto Csv<Type, Types...>::convertToCxx(const std::smatch& result) noexcept
    -> FieldType<index>
{
  return JsonValueParser::toCxxTypeValue<FieldType<index>>(result.str(index + 1));
}

/*!
  \details
  No detailed.
  */
template <typename Type, typename ...Types> inline
auto Csv<Type, Types...>::parseCsvLine(
    const std::string& line,
    std::list<std::string>* message_list) const noexcept -> RecordType
{
  std::smatch result;
  const auto is_success = std::regex_match(line, result, csvRegex());
  if ((message_list != nullptr) && !is_success) {
    std::ostringstream error;
    error << R"(Parsing ")" << line << R"(" failed.)" << std::endl;
    message_list->emplace_back(error.str());
  }
  return (is_success)
      ? toCxx(result, std::make_index_sequence<columnSize()>())
      : RecordType{};
}

/*!
  */
template <typename Type, typename ...Types> template <std::size_t ...indices> inline
auto Csv<Type, Types...>::toCxx(const std::smatch& result,
                                std::index_sequence<indices...>) noexcept
    -> RecordType
{
  return std::make_tuple(convertToCxx<indices>(result)...);
}

} // namespace zisc

#endif // ZISC_CSV_INL_HPP
