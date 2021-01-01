/*!
  \file csv.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CSV_HPP
#define ZISC_CSV_HPP

// Standard C++ library
#include <cstddef>
#include <istream>
#include <regex>
#include <string_view>
#include <tuple>
#include <vector>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

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
  Csv(pmr::memory_resource* mem_resource) noexcept;

  //! Move data
  Csv(Csv&& other) noexcept;


  //! Move data
  Csv& operator=(Csv&& other) noexcept;


  //! Add values
  template <std::size_t kMaxCharsPerLine = 256>
  void append(std::istream& csv) noexcept;

  //! Return the number of elements that can be held in allocated storage
  std::size_t capacity() const noexcept;

  //! Clear all csv data
  void clear() noexcept;

  //! Return the column size
  static constexpr uint columnSize() noexcept;

  //! Return the pattern string for regex
  static constexpr auto csvPattern() noexcept;

  //! Return the CSV regex
  const std::regex& csvRegex() const noexcept;

  //! Return the data of the csv
  const pmr::vector<RecordType>& data() const noexcept;

  //! Return the field value of the given row by the column index
  template <std::size_t column>
  const FieldType<column> get(const std::size_t row) const noexcept;

  //! Return the record by the row
  const RecordType& record(const std::size_t row) const noexcept;

  //! Return the row size
  std::size_t rowSize() const noexcept;

  //! Reserve storage
  void setCapacity(const std::size_t cap) noexcept;

 private:
  template <std::size_t index>
  using InnerFieldType = typename std::tuple_element<index, RecordType>::type;


  //! Convert a CSV bool to a C++ bool 
  template <SameAs<bool> PType>
  PType toCxxType(const std::string_view json_value) const noexcept;

  //! Convert a CSV float to a C++ float
  template <FloatingPoint PType>
  PType toCxxType(const std::string_view json_value) const noexcept;

  //! Convert a CSV integer to a C++ integer
  template <Integer PType>
  PType toCxxType(const std::string_view json_value) const noexcept;

  //! Convert a CSV string to a C++ string
  template <String PType>
  pmr::string toCxxType(const std::string_view json_value) noexcept;

  //! Return the pattern string for regex
  template <typename PType, typename ...PTypes>
  static constexpr auto getCsvPattern() noexcept;

  //! Return the boolean pattern string for regex
  template <SameAs<bool> PType>
  static constexpr auto getTypePattern() noexcept;

  //! Return the float pattern string for regex
  template <FloatingPoint PType>
  static constexpr auto getTypePattern() noexcept;

  //! Return the integer pattern string for regex
  template <Integer PType>
  static constexpr auto getTypePattern() noexcept;

  //! Return the string pattern string for regex
  template <String PType>
  static constexpr auto getTypePattern() noexcept;

  //! Return the underlying memory resource
  pmr::memory_resource* memoryResource() noexcept;

  //! Parse csv line
  RecordType parseCsvLine(std::string_view line) noexcept;

  //! Convert a CSV line to C++ values
  template <std::size_t ...indices>
  RecordType toCxxRecord(const pmr::cmatch& result,
                         std::index_sequence<indices...>) noexcept;

  //! Convert a CSV value to a C++ value
  template <std::size_t index>
  InnerFieldType<index> toCxxField(const pmr::cmatch& result) noexcept;


  pmr::vector<RecordType> data_;
  std::regex csv_pattern_;
};

} // namespace zisc

/*!
  \example csv_example.cpp

  This is an example of how to use zisc::Csv.
  */

#include "csv-inl.hpp"

#endif // ZISC_CSV_HPP
