/*!
  \file string.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STRING_HPP
#define ZISC_STRING_HPP

// Standard C++ library
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Constexprable string class

  No detailed description.

  \tparam CharType No description.
  \tparam kN No description.

  \todo Implement explicit conversion operator
  */
template <typename CharType, uint kN>
class BasicString
{
  static_assert(0 < kN, "The N is zero.");
  using ArrayType = std::array<CharType, kN>;

 public:
  //! Type aliases for STL
  using value_type = typename ArrayType::value_type;
  using size_type = uint;
  using difference_type = typename ArrayType::difference_type;
  using reference = typename ArrayType::reference;
  using const_reference = typename ArrayType::const_reference;
  using pointer = typename ArrayType::pointer;
  using const_pointer = typename ArrayType::const_pointer;
  using iterator = typename ArrayType::iterator;
  using const_iterator = typename ArrayType::const_iterator;
  using reverse_iterator = typename ArrayType::reverse_iterator;
  using const_reverse_iterator = typename ArrayType::const_reverse_iterator;


  //! Create a empty string
  constexpr BasicString() noexcept;

  //! Create a string
  constexpr BasicString(const CharType (&other)[kN]) noexcept;


  //! For STL
  //! Return an iterator to the first element of the containter
  constexpr iterator begin() noexcept;

  //! Return an iterator to the first element of the containter
  constexpr const_iterator begin() const noexcept;

  //! Return an iterator to the first element of the containter
  constexpr const_iterator cbegin() const noexcept;

  //! Return an iterator following the last element of the container
  constexpr iterator end() noexcept;

  //! Return an iterator following the last element of the container
  constexpr const_iterator end() const noexcept;

  //! Return an iterator following the last element of the container
  constexpr const_iterator cend() const noexcept;


  //! Access specified character
  constexpr reference operator[](const uint index) noexcept;

  //! Access specified character
  constexpr const_reference operator[](const uint index) const noexcept;


  //! Return the reference of the array
  constexpr ArrayType& data() noexcept;

  //! Return the reference of the array
  constexpr const ArrayType& data() const noexcept;

  //! Access specified character
  constexpr reference get(const uint index) noexcept;

  //! Access specified character
  constexpr const_reference get(const uint index) const noexcept;

  //! Returns the number of CharType elements in the string
  constexpr size_type size() const noexcept;

  //! Return a standard c character array
  constexpr const_pointer toCString() const noexcept;

  //! Convert to std::basic_string<CharType>
  std::basic_string<CharType> toStdString() const noexcept;

  //! Convert to std::basic_string_view<CharType>
  std::basic_string_view<CharType> toStdStringView() const noexcept;

 private:
  //! Make a array
  template <std::size_t ...indices>
  static constexpr ArrayType makeArray(const CharType (&other)[kN],
                                       std::index_sequence<indices...>) noexcept;

  //! Make a empty array
  static constexpr ArrayType makeEmptyArray() noexcept;


  ArrayType data_;
};

// Constexpr string aliases
template <uint kN>
using String = BasicString<char, kN>;

//! Get the length of string
constexpr std::size_t getSize(const char* s) noexcept;

//! Concatenates two strings
template <typename CharType, uint kN1, uint kN2>
constexpr BasicString<CharType, kN1 + kN2 - 1> operator+(
    const BasicString<CharType, kN1>& lhs,
    const BasicString<CharType, kN2>& rhs) noexcept;

//! Concatenates two strings
template <typename CharType, uint kN1, uint kN2>
constexpr BasicString<CharType, kN1 + kN2 - 1> operator+(
    const CharType (&lhs)[kN1],
    const BasicString<CharType, kN2>& rhs) noexcept;

//! Concatenates two strings
template <typename CharType, uint kN1, uint kN2>
constexpr BasicString<CharType, kN1 + kN2 - 1> operator+(
    const BasicString<CharType, kN1>& lhs,
    const CharType (&rhs)[kN2]) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN1, uint kN2>
constexpr bool operator==(
    const BasicString<CharType, kN1>& lhs,
    const BasicString<CharType, kN2>& rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN>
constexpr bool operator==(
    const CharType* lhs,
    const BasicString<CharType, kN>& rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN>
constexpr bool operator==(
    const BasicString<CharType, kN>& lhs,
    const CharType* rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN>
constexpr bool operator==(
    const std::basic_string_view<CharType>& lhs,
    const BasicString<CharType, kN>& rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN>
constexpr bool operator==(
    const BasicString<CharType, kN>& lhs,
    const std::basic_string_view<CharType>& rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN1, uint kN2>
constexpr bool operator!=(
    const BasicString<CharType, kN1>& lhs,
    const BasicString<CharType, kN2>& rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN>
constexpr bool operator!=(
    const CharType* lhs,
    const BasicString<CharType, kN>& rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN>
constexpr bool operator!=(
    const BasicString<CharType, kN>& lhs,
    const CharType* rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN>
constexpr bool operator!=(
    const std::basic_string_view<CharType>& lhs,
    const BasicString<CharType, kN>& rhs) noexcept;

//! Lexicographically compares two strings
template <typename CharType, uint kN>
constexpr bool operator!=(
    const BasicString<CharType, kN>& lhs,
    const std::basic_string_view<CharType>& rhs) noexcept;

//! Concatenates two strings
template <typename CharType, uint kN1, uint kN2>
constexpr BasicString<CharType, kN1 + kN2 - 1> concatenate(
    const BasicString<CharType, kN1>& lhs, 
    const BasicString<CharType, kN2>& rhs) noexcept;

//! Concatenates two strings
template <typename CharType, uint kN1, uint kN2>
constexpr BasicString<CharType, kN1 + kN2 - 1> concatenate(
    const CharType (&lhs)[kN1],
    const BasicString<CharType, kN2>& rhs) noexcept;

//! Concatenates two strings
template <typename CharType, uint kN1, uint kN2>
constexpr BasicString<CharType, kN1 + kN2 - 1> concatenate(
    const BasicString<CharType, kN1>& lhs, 
    const CharType (&rhs)[kN2]) noexcept;

//! Convert the string to String class
template <typename CharType, uint kN>
constexpr BasicString<CharType, kN> toString(const CharType (&other)[kN]) noexcept;

} // namespace zisc

/*!
  \example string_example.cpp

  This is an example of how to use zisc::BasicString.
  */

#include "string-inl.hpp"

#endif // ZISC_STRING_HPP
