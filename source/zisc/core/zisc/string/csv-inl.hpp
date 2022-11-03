/*!
  \file csv-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CSV_INL_HPP
#define ZISC_CSV_INL_HPP

#include "csv.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <istream>
#include <regex>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "constant_string.hpp"
#include "json_value_parser.hpp"
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  \tparam Types No description.
  */
template <typename Type, typename ...Types>
struct CsvRecordTypeImpl<0, Type, Types...>
{
  using RecordT = std::tuple<Type, Types...>;
};

/*!
  \brief No brief description

  No detailed description.

  \tparam kN No description.
  \tparam Type No description.
  \tparam Types No description.
  */
template <std::size_t kN, typename Type, typename ...Types>
struct CsvRecordTypeImpl
{
  using T = std::conditional_t<String<Type>, pmr::string, Type>;
  using RecordT = typename CsvRecordTypeImpl<kN - 1, Types..., T>::RecordT;
};

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <typename Type, typename ...Types> inline
Csv<Type, Types...>::Csv(pmr::memory_resource* mem_resource) noexcept : 
    data_{typename decltype(data_)::allocator_type{mem_resource}},
    csv_pattern_{csvPattern().toCStr(),
                 std::regex_constants::optimize | std::regex_constants::ECMAScript}
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

  \tparam kMaxNumCharsPerLine No description.
  \param [in,out] csv No description.
  */
template <typename Type, typename ...Types> template <std::size_t kMaxNumCharsPerLine>
inline
void Csv<Type, Types...>::append(std::istream& csv) noexcept
{
  char s[kMaxNumCharsPerLine];
  constexpr std::size_t n = kMaxNumCharsPerLine;
  for (csv.getline(s, n); !csv.eof(); csv.getline(s, n)) {
    std::string_view line{s};
    const RecordType cxx_record = parseCsvLine(line);
    data_.emplace_back(std::move(cxx_record));
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
template <typename Type, typename ...Types> template <std::size_t column> inline
auto Csv<Type, Types...>::get(const std::size_t row) const noexcept
    -> const FieldType<column>
{
  const RecordType& record = this->record(row);
  const std::tuple_element_t<column, RecordType>& field = std::get<column>(record);
  using FieldT = FieldType<column>;
  if constexpr (CharPointer<FieldT>) {
    static_assert(String<InnerFieldType<column>>);
    const FieldT result = field.c_str();
    return result;
  }
  else {
    const FieldT result = field;
    return result;
  }
}

/*!
  \details No detailed description

  \param [in] row No description.
  \return No description
  */
template <typename Type, typename ...Types> inline
auto Csv<Type, Types...>::record(const std::size_t row) const noexcept
    -> const RecordType&
{
  //! \todo Exception check
  ZISC_ASSERT(row < rowSize(), "The row is out of range.");
  const RecordType& record = data_[row];
  return record;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename ...Types> inline
std::size_t Csv<Type, Types...>::rowSize() const noexcept
{
  const std::size_t s = data_.size();
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

  \tparam PType No description.
  \param [in] json_value No description.
  \return No description
  */
template <typename Type, typename ...Types> template <std::same_as<bool> PType> inline
PType Csv<Type, Types...>::toCxxType(const std::string_view json_value) const noexcept
{
  const PType result = JsonValueParser::toCxxBool(json_value);
  return result;
}

/*!
  \details No detailed description

  \tparam PType No description.
  \param [in] json_value No description.
  \return No description
  */
template <typename Type, typename ...Types> template <std::floating_point PType> inline
PType Csv<Type, Types...>::toCxxType(const std::string_view json_value) const noexcept
{
  const PType result = JsonValueParser::toCxxFloat<PType>(json_value);
  return result;
}

/*!
  \details No detailed description

  \tparam PType No description.
  \param [in] json_value No description.
  \return No description
  */
template <typename Type, typename ...Types> template <Integer PType> inline
PType Csv<Type, Types...>::toCxxType(const std::string_view json_value) const noexcept
{
  const PType result = JsonValueParser::toCxxInteger<PType>(json_value);
  return result;
}

/*!
  \details No detailed description

  \tparam PType No description.
  \param [in] json_value No description.
  \return No description
  */
template <typename Type, typename ...Types> template <String PType> inline
pmr::string Csv<Type, Types...>::toCxxType(const std::string_view json_value) noexcept
{
  pmr::string::allocator_type alloc{memoryResource()};
  pmr::string result{alloc};

  const std::size_t size = JsonValueParser::getCxxStringSize(json_value);
  result.resize(size);

  JsonValueParser::toCxxString(json_value, result.data());
  return result;
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
  if constexpr (rest == 0) {
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
template <typename Type, typename ...Types> template <std::same_as<bool> PType> inline
constexpr auto Csv<Type, Types...>::getTypePattern() noexcept
{
  return JsonValueParser::booleanPattern();
}

/*!
  \details No detailed description

  \tparam PType No description.
  \return No description
  */
template <typename Type, typename ...Types> template <std::floating_point PType> inline
constexpr auto Csv<Type, Types...>::getTypePattern() noexcept
{
  return JsonValueParser::floatPattern();
}

/*!
  \details No detailed description

  \tparam PType No description.
  \return No description
  */
template <typename Type, typename ...Types> template <Integer PType> inline
constexpr auto Csv<Type, Types...>::getTypePattern() noexcept
{
  return JsonValueParser::integerPattern();
}

/*!
  \details No detailed description

  \tparam PType No description.
  \return No description
  */
template <typename Type, typename ...Types> template <String PType> inline
constexpr auto Csv<Type, Types...>::getTypePattern() noexcept
{
  return JsonValueParser::stringPattern();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename ...Types> inline
pmr::memory_resource* Csv<Type, Types...>::memoryResource() noexcept
{
  auto mem_resource = data_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] line No description.
  \return No description
  */
template <typename Type, typename ...Types> inline
auto Csv<Type, Types...>::parseCsvLine(std::string_view line) noexcept
    -> RecordType
{
  //! \todo Exception check
  pmr::cmatch::allocator_type alloc{memoryResource()};
  pmr::cmatch result{alloc};
  [[maybe_unused]] const auto is_success = std::regex_match(line.data(),
                                                            result,
                                                            csvRegex());
//  if (!is_success) {
//    error << R"(Parsing ")" << line << R"(" failed.)" << std::endl;
//  }
  auto cxx_record = toCxxRecord(result, std::make_index_sequence<columnSize()>());
  return cxx_record;
}

/*!
  \details No detailed description

  \tparam indices No description.
  \param [in] result No description.
  \return No description
  */
template <typename Type, typename ...Types>
template <std::size_t ...indices> inline
auto Csv<Type, Types...>::toCxxRecord(const pmr::cmatch& result,
                                      std::index_sequence<indices...>) noexcept
    -> RecordType
{
  const RecordType cxx_record = std::make_tuple(toCxxField<indices>(result)...);
  return cxx_record;
}

/*!
  \details No detailed description

  \tparam index No description.
  \param [in] result No description.
  \return No description
  */
template <typename Type, typename ...Types> template <std::size_t index> inline
auto Csv<Type, Types...>::toCxxField(const pmr::cmatch& result) noexcept
    -> InnerFieldType<index>
{
  using FieldT = InnerFieldType<index>;
  pmr::cmatch::const_reference match = result[index + 1];
  const auto fieldsize = cast<std::size_t>(std::distance(match.first, match.second));
  std::string_view field{match.first, fieldsize};
  auto cxx_field = toCxxType<FieldT>(field);
  return cxx_field;
}

} // namespace zisc

#endif // ZISC_CSV_INL_HPP
