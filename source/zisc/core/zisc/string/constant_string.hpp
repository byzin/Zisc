/*!
  \file constant_string.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONSTANT_STRING_HPP
#define ZISC_CONSTANT_STRING_HPP

// Standard C++ library
#include <array>
#include <cstddef>
#include <string_view>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Constexprable string class

  No detailed description.

  \tparam Char No description.
  \tparam kN No description.

  \todo Implement explicit conversion operator
  */
template <Character Char, uint kN>
class BasicConstantString
{
  static_assert(0 < kN, "The N is zero.");

 public:
  // Types
  using ArrayType = std::array<Char, kN>;
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
  constexpr BasicConstantString() noexcept;

  //! Create a string
  constexpr BasicConstantString(const value_type (&other)[kN]) noexcept;


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

  //! Returns the number of Char elements in the string
  constexpr size_type size() const noexcept;

  //! Return a standard c character array
  const_pointer toCStr() const noexcept;

  //! Convert to std::basic_string_view<Char>
  std::basic_string_view<value_type> toStdStr() const noexcept;

 private:
  //! Make a array
  template <std::size_t ...indices>
  static constexpr ArrayType makeArray(const value_type (&other)[kN],
                                       std::index_sequence<indices...>) noexcept;

  //! Make an empty array
  static constexpr ArrayType makeEmptyArray() noexcept;


  ArrayType data_;
};

// Constexpr string aliases
template <uint kN>
using CString = BasicConstantString<char, kN>;
template <uint kN>
using WCString = BasicConstantString<wchar_t, kN>;
template <uint kN>
using U8CString = BasicConstantString<char8_t, kN>;
template <uint kN>
using U16CString = BasicConstantString<char16_t, kN>;
template <uint kN>
using U32CString = BasicConstantString<char32_t, kN>;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr BasicConstantString<Char, kN1 + kN2 - 1> operator+(
    const BasicConstantString<Char, kN1>& lhs,
    const BasicConstantString<Char, kN2>& rhs) noexcept;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr BasicConstantString<Char, kN1 + kN2 - 1> operator+(
    const Char (&lhs)[kN1],
    const BasicConstantString<Char, kN2>& rhs) noexcept;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr BasicConstantString<Char, kN1 + kN2 - 1> operator+(
    const BasicConstantString<Char, kN1>& lhs,
    const Char (&rhs)[kN2]) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN1, uint kN2>
constexpr bool operator==(const BasicConstantString<Char, kN1>& lhs,
                          const BasicConstantString<Char, kN2>& rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr bool operator==(const Char* lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr bool operator==(const BasicConstantString<Char, kN>& lhs,
                          const Char* rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr bool operator==(const std::basic_string_view<Char>& lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr bool operator==(const BasicConstantString<Char, kN>& lhs,
                          const std::basic_string_view<Char>& rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN1, uint kN2>
constexpr bool operator!=(const BasicConstantString<Char, kN1>& lhs,
                          const BasicConstantString<Char, kN2>& rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr bool operator!=(const Char* lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr bool operator!=(const BasicConstantString<Char, kN>& lhs,
                          const Char* rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr bool operator!=(const std::basic_string_view<Char>& lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr bool operator!=(const BasicConstantString<Char, kN>& lhs,
                          const std::basic_string_view<Char>& rhs) noexcept;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr BasicConstantString<Char, kN1 + kN2 - 1> concatenate(
    const BasicConstantString<Char, kN1>& lhs, 
    const BasicConstantString<Char, kN2>& rhs) noexcept;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr BasicConstantString<Char, kN1 + kN2 - 1> concatenate(
    const Char (&lhs)[kN1],
    const BasicConstantString<Char, kN2>& rhs) noexcept;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr BasicConstantString<Char, kN1 + kN2 - 1> concatenate(
    const BasicConstantString<Char, kN1>& lhs, 
    const Char (&rhs)[kN2]) noexcept;

//! Convert the string to String class
template <Character Char, uint kN>
constexpr BasicConstantString<Char, kN> toString(const Char (&other)[kN]) noexcept;

} // namespace zisc

/*!
  \example string_example.cpp

  This is an example of how to use zisc::BasicConstantString.
  */

#include "constant_string-inl.hpp"

#endif // ZISC_CONSTANT_STRING_HPP
