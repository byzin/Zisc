/*!
  \file csv.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CSV_HPP
#define ZISC_CSV_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <istream>
#include <memory_resource>
#include <regex>
#include <span>
#include <string_view>
#include <tuple>
#include <vector>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
template <std::size_t kN, typename Type, typename ...Types> struct CsvRecordTypeImpl;

/*!
  \brief Manipulate CSV file

  No detailed description.

  \tparam Type No description.
  \tparam Types No description.

  \todo Add tests
  */
template <typename Type, typename ...Types>
class Csv : private NonCopyable<Csv<Type, Types...>>
{
 public:
  //! Represent values in a line
  using RecordType = typename CsvRecordTypeImpl<sizeof...(Types) + 1,
                                                Type,
                                                Types...>::RecordT;
  //! Represent a value in a line by the index
  template <std::size_t index>
  using FieldType = typename std::tuple_element<index,
                                                std::tuple<Type, Types...>>::type;


  //! Initialize CSV
  explicit Csv(std::pmr::memory_resource* mem_resource) noexcept;

  //! Move data
  Csv(Csv&& other) noexcept;


  //! Move data
  auto operator=(Csv&& other) noexcept -> Csv&;


  //! Add values
  template <std::size_t kMaxCharsPerLine = 256>
  void append(std::istream& csv) noexcept;

  //! Return the number of elements that can be held in allocated storage
  [[nodiscard]]
  auto capacity() const noexcept -> std::size_t;

  //! Clear all csv data
  void clear() noexcept;

  //! Return the column size
  static constexpr auto columnSize() noexcept -> uint;

  //! Return the pattern string for regex
  static constexpr auto csvPattern() noexcept;

  //! Return the CSV regex
  [[nodiscard]]
  auto csvRegex() const noexcept -> const std::regex&;

  //! Return the data of the csv
  auto data() const noexcept -> std::span<const RecordType>;

  //! Return the field value of the given row by the column index
  template <std::size_t column>
  auto get(const std::size_t row) const noexcept -> FieldType<column>;

  //! Return the record by the row
  auto record(const std::size_t row) const noexcept -> const RecordType&;

  //! Return the row size
  [[nodiscard]]
  auto rowSize() const noexcept -> std::size_t;

  //! Reserve storage
  void setCapacity(const std::size_t cap) noexcept;

 private:
  template <std::size_t index>
  using InnerFieldType = typename std::tuple_element<index, RecordType>::type;


  //! Convert a CSV bool to a C++ bool 
  template <std::same_as<bool> PType>
  auto toCxxType(const std::string_view json_value) const noexcept -> PType;

  //! Convert a CSV float to a C++ float
  template <std::floating_point PType>
  auto toCxxType(const std::string_view json_value) const noexcept -> PType;

  //! Convert a CSV integer to a C++ integer
  template <Integer PType>
  auto toCxxType(const std::string_view json_value) const noexcept -> PType;

  //! Convert a CSV string to a C++ string
  template <String PType>
  auto toCxxType(const std::string_view json_value) noexcept -> std::pmr::string;

  //! Return the pattern string for regex
  template <typename PType, typename ...PTypes>
  static constexpr auto getCsvPattern() noexcept;

  //! Return the boolean pattern string for regex
  template <std::same_as<bool> PType>
  static constexpr auto getTypePattern() noexcept;

  //! Return the float pattern string for regex
  template <std::floating_point PType>
  static constexpr auto getTypePattern() noexcept;

  //! Return the integer pattern string for regex
  template <Integer PType>
  static constexpr auto getTypePattern() noexcept;

  //! Return the string pattern string for regex
  template <String PType>
  static constexpr auto getTypePattern() noexcept;

  //! Return the underlying memory resource
  auto memoryResource() noexcept -> std::pmr::memory_resource*;

  //! Parse csv line
  auto parseCsvLine(std::string_view line) noexcept -> RecordType;

  //! Convert a CSV line to C++ values
  template <std::size_t ...indices>
  auto toCxxRecord(const std::pmr::cmatch& result,
                   std::index_sequence<indices...> idx) noexcept -> RecordType;

  //! Convert a CSV value to a C++ value
  template <std::size_t index>
  auto toCxxField(const std::pmr::cmatch& result) noexcept -> InnerFieldType<index>;


  std::pmr::vector<RecordType> data_;
  std::regex csv_pattern_;
};

} // namespace zisc

/*!
  \example csv_example.cpp

  This is an example of how to use zisc::Csv.
  */

#include "csv-inl.hpp"

#endif // ZISC_CSV_HPP
