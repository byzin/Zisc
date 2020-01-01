/*!
  \file csv.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CSV_HPP
#define ZISC_CSV_HPP

// Standard C++ library
#include <cstddef>
#include <istream>
#include <regex>
#include <string>
#include <tuple>
#include <vector>
// Zisc
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"
#include "string.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

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
  using RecordType = std::tuple<Type, Types...>;
  //! Represent a value in a line by the index
  template <uint index>
  using FieldType = typename std::tuple_element<index, RecordType>::type;


  //! Initialize CSV
  Csv(std::pmr::memory_resource* mem_resource) noexcept;

  //! Move data
  Csv(Csv&& other) noexcept;


  //! Move data
  Csv& operator=(Csv&& other) noexcept;


  //! Add values
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
  template <uint column>
  const FieldType<column>& get(const uint row) const noexcept;

  //! Return the record by the row
  const RecordType& record(const uint row) const noexcept;

  //! Return the row size
  uint rowSize() const noexcept;

  //! Reserve storage
  void setCapacity(const std::size_t cap) noexcept;

 private:
  //! Convert a CSV value to a C++ value
  template <std::size_t index>
  static FieldType<index> convertToCxx(const std::smatch& result) noexcept;

  //! Return the pattern string for regex
  template <typename PType, typename ...PTypes>
  static constexpr auto getCsvPattern() noexcept;

  //! Return the pattern string for regex
  template <typename PType>
  static constexpr auto getTypePattern() noexcept;

  //! Parse csv line
  RecordType parseCsvLine(const std::string& line) const noexcept;

  //! Convert a CSV line to C++ values
  template <std::size_t ...indices>
  static RecordType toCxx(const std::smatch& result,
                          std::index_sequence<indices...>) noexcept;


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
