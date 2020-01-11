/*!
  \file csv-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CSV_INL_HPP
#define ZISC_CSV_INL_HPP

#include "csv.hpp"
// Standard C++ library
#include <istream>
#include <regex>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>
// Zisc
#include "error.hpp"
#include "json_value_parser.hpp"
#include "std_memory_resource.hpp"
#include "string.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <typename Type, typename ...Types> inline
Csv<Type, Types...>::Csv(std::pmr::memory_resource* mem_resource) noexcept : 
    data_{typename pmr::vector<RecordType>::allocator_type{mem_resource}},
    csv_pattern_{csvPattern().toCString(),
                 std::regex_constants::optimize|std::regex_constants::ECMAScript}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type, typename ...Types> inline
Csv<Type, Types...>::Csv(Csv&& other) noexcept : 
    data_{std::move(other.data_)},
    csv_pattern_{std::move(other.csv_pattern_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type, typename ...Types> inline
auto Csv<Type, Types...>::operator=(Csv&& other) noexcept -> Csv&
{
  data_ = std::move(other.data_);
  csv_pattern_ = std::move(other.csv_pattern_);
  return *this;
}

/*!
  \details No detailed description

  \param [in,out] csv No description.
  */
template <typename Type, typename ...Types> inline
void Csv<Type, Types...>::append(std::istream& csv) noexcept
{
  for (std::string line; std::getline(csv, line);) {
    auto record = parseCsvLine(line);
    data_.emplace_back(std::move(record));
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename ...Types> inline
std::size_t Csv<Type, Types...>::capacity() const noexcept
{
  const std::size_t cap = data_.capacity();
  return cap;
}

/*!
  \details No detailed description
  */
template <typename Type, typename ...Types> inline
void Csv<Type, Types...>::clear() noexcept
{
  data_.clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename ...Types> inline
constexpr uint Csv<Type, Types...>::columnSize() noexcept
{
  constexpr uint size = 1 + sizeof...(Types);
  return size;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename ...Types> inline
constexpr auto Csv<Type, Types...>::csvPattern() noexcept
{
  constexpr auto pattern = getCsvPattern<Type, Types...>();
  return pattern;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename ...Types> inline
const std::regex& Csv<Type, Types...>::csvRegex() const noexcept
{
  return csv_pattern_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename ...Types> inline
auto Csv<Type, Types...>::data() const noexcept -> const pmr::vector<RecordType>&
{
  return data_;
}

/*!
  \details No detailed description

  \tparam column No description.
  \param [in] row No description.
  \return No description
  */
template <typename Type, typename ...Types> template <uint column> inline
auto Csv<Type, Types...>::get(const uint row) const noexcept
    -> const FieldType<column>&
{
  const auto& record = this->record(row);
  const FieldType<column>& field = std::get<column>(record);
  return field;
}

/*!
  \details No detailed description

  \param [in] row No description.
  \return No description
  */
template <typename Type, typename ...Types> inline
auto Csv<Type, Types...>::record(const uint row) const noexcept
    -> const RecordType&
{
  //! \todo Exception check
  ZISC_ASSERT(row < rowSize(), "The row is out of range.");
  auto record = data_.begin();
  std::advance(record, row);
  return *record;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename ...Types> inline
uint Csv<Type, Types...>::rowSize() const noexcept
{
  const uint s = cast<uint>(data_.size());
  return s;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <typename Type, typename ...Types> inline
void Csv<Type, Types...>::setCapacity(const std::size_t cap) noexcept
{
  data_.reserve(cap);
}

/*!
  \details No detailed description

  \tparam index No description.
  \param [in] result No description.
  \return No description
  */
template <typename Type, typename ...Types> template <std::size_t index> inline
auto Csv<Type, Types...>::convertToCxx(const std::smatch& result) noexcept
    -> FieldType<index>
{
  using FieldT = FieldType<index>;
  const auto csv_field = result.str(index + 1);
  auto cxx_field = JsonValueParser::toCxxTypeValue<FieldT>(csv_field);
  return cxx_field;
}

/*!
  \details No detailed description

  \tparam PType No description.
  \tparam PTypes No description.
  \return No description
  */
template <typename Type, typename ...Types>
template <typename PType, typename ...PTypes> inline
constexpr auto Csv<Type, Types...>::getCsvPattern() noexcept
{
  constexpr std::size_t rest = sizeof...(PTypes);
  const auto pattern1 = R"(\s*()" + getTypePattern<PType>() + R"()\s*)";
  if constexpr (0 == rest) {
    return pattern1;
  }
  else {
    const auto pattern2 = pattern1 + R"(,)" + getCsvPattern<PTypes...>();
    return pattern2;
  }
}

/*!
  \details No detailed description

  \tparam PType No description.
  \return No description
  */
template <typename Type, typename ...Types>
template <typename PType> inline
constexpr auto Csv<Type, Types...>::getTypePattern() noexcept
{
  if constexpr (kIsBoolean<PType>) {
    return JsonValueParser::booleanPattern();
  }
  else if constexpr (kIsInteger<PType>) {
    return JsonValueParser::integerPattern();
  }
  else if constexpr (kIsFloat<PType>) {
    return JsonValueParser::floatPattern();
  }
  else if constexpr (std::is_same_v<std::string, PType>) {
    return JsonValueParser::stringPattern();
  }
  else {
    static_assert(kIsBoolean<PType>, "Unsupported PType is specified.");
    return 0;
  }
}

/*!
  \details No detailed description

  \param [in] line No description.
  \return No description
  */
template <typename Type, typename ...Types> inline
auto Csv<Type, Types...>::parseCsvLine(const std::string& line) const noexcept
    -> RecordType
{
  //! \todo Exception check
  std::smatch result;
  const auto is_success = std::regex_match(line, result, csvRegex());
//  if (!is_success) {
//    error << R"(Parsing ")" << line << R"(" failed.)" << std::endl;
//  }
  static_cast<void>(is_success);
  auto field = toCxx(result, std::make_index_sequence<columnSize()>());
  return field;
}

/*!
  \details No detailed description

  \tparam indices No description.
  \param [in] result No description.
  \return No description
  */
template <typename Type, typename ...Types>
template <std::size_t ...indices> inline
auto Csv<Type, Types...>::toCxx(const std::smatch& result,
                                std::index_sequence<indices...>) noexcept
    -> RecordType
{
  auto cxx_field = std::make_tuple(convertToCxx<indices>(result)...);
  return cxx_field;
}

} // namespace zisc

#endif // ZISC_CSV_INL_HPP
