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
  explicit constexpr BasicConstantString(const value_type (&other)[kN]) noexcept;


  //! For STL
  //! Return an iterator to the first element of the containter
  [[nodiscard]]
  constexpr auto begin() noexcept -> iterator;

  //! Return an iterator to the first element of the containter
  [[nodiscard]]
  constexpr auto begin() const noexcept -> const_iterator;

  //! Return an iterator to the first element of the containter
  [[nodiscard]]
  constexpr auto cbegin() const noexcept -> const_iterator;

  //! Return an iterator following the last element of the container
  [[nodiscard]]
  constexpr auto end() noexcept -> iterator;

  //! Return an iterator following the last element of the container
  [[nodiscard]]
  constexpr auto end() const noexcept -> const_iterator;

  //! Return an iterator following the last element of the container
  [[nodiscard]]
  constexpr auto cend() const noexcept -> const_iterator;


  //! Access specified character
  constexpr auto operator[](const uint index) noexcept -> reference;

  //! Access specified character
  constexpr auto operator[](const uint index) const noexcept -> const_reference;


  //! Return the reference of the array
  [[nodiscard]]
  constexpr auto data() noexcept -> ArrayType&;

  //! Return the reference of the array
  [[nodiscard]]
  constexpr auto data() const noexcept -> const ArrayType&;

  //! Access specified character
  [[nodiscard]]
  constexpr auto get(const uint index) noexcept -> reference;

  //! Access specified character
  [[nodiscard]]
  constexpr auto get(const uint index) const noexcept -> const_reference;

  //! Returns the number of Char elements in the string
  [[nodiscard]]
  constexpr auto size() const noexcept -> size_type;

  //! Return a standard c character array
  [[nodiscard]]
  auto toCStr() const noexcept -> const_pointer;

  //! Convert to std::basic_string_view<Char>
  [[nodiscard]]
  auto toStdStr() const noexcept -> std::basic_string_view<value_type>;

 private:
  //! Make a array
  template <std::size_t ...indices>
  static constexpr auto makeArray(const value_type (&other)[kN],
                                  std::index_sequence<indices...> idx) noexcept -> ArrayType;

  //! Make an empty array
  static constexpr auto makeEmptyArray() noexcept -> ArrayType;


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
constexpr auto operator+(const BasicConstantString<Char, kN1>& lhs,
                         const BasicConstantString<Char, kN2>& rhs) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr auto operator+(const Char (&lhs)[kN1],
                         const BasicConstantString<Char, kN2>& rhs) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr auto operator+(const BasicConstantString<Char, kN1>& lhs,
                         const Char (&rhs)[kN2]) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>;

//! Lexicographically compares two strings
template <Character Char, uint kN1, uint kN2>
constexpr auto operator==(const BasicConstantString<Char, kN1>& lhs,
                          const BasicConstantString<Char, kN2>& rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr auto operator==(const Char* lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr auto operator==(const BasicConstantString<Char, kN>& lhs,
                          const Char* rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr auto operator==(const std::basic_string_view<Char>& lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr auto operator==(const BasicConstantString<Char, kN>& lhs,
                          const std::basic_string_view<Char>& rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN1, uint kN2>
constexpr auto operator!=(const BasicConstantString<Char, kN1>& lhs,
                          const BasicConstantString<Char, kN2>& rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr auto operator!=(const Char* lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr auto operator!=(const BasicConstantString<Char, kN>& lhs,
                          const Char* rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr auto operator!=(const std::basic_string_view<Char>& lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept -> bool;

//! Lexicographically compares two strings
template <Character Char, uint kN>
constexpr auto operator!=(const BasicConstantString<Char, kN>& lhs,
                          const std::basic_string_view<Char>& rhs) noexcept -> bool;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr auto concatenate(const BasicConstantString<Char, kN1>& lhs, 
                           const BasicConstantString<Char, kN2>& rhs) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr auto concatenate(const Char (&lhs)[kN1],
                           const BasicConstantString<Char, kN2>& rhs) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>;

//! Concatenates two strings
template <Character Char, uint kN1, uint kN2>
constexpr auto concatenate(const BasicConstantString<Char, kN1>& lhs, 
                           const Char (&rhs)[kN2]) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>;

//! Convert the string to String class
template <Character Char, uint kN>
constexpr auto toString(const Char (&other)[kN]) noexcept -> BasicConstantString<Char, kN>;

} // namespace zisc

/*!
  \example string_example.cpp

  This is an example of how to use zisc::BasicConstantString.
  */

#include "constant_string-inl.hpp"

#endif // ZISC_CONSTANT_STRING_HPP
