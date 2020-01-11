/*!
  \file string-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_STRING_INL_HPP
#define ZISC_STRING_INL_HPP

#include "string.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <string_view>
#include <utility>
// Zisc
#include "error.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename CharType, uint kN> inline
constexpr BasicString<CharType, kN>::BasicString() noexcept
    : data_{makeEmptyArray()}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename CharType, uint kN> inline
constexpr BasicString<CharType, kN>::BasicString(
    const CharType (&other)[kN]) noexcept
    : data_{makeArray(other, std::make_index_sequence<kN>())}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::begin() noexcept -> iterator
{
  return data_.begin();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::begin() const noexcept -> const_iterator
{
  return data_.begin();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::cbegin() const noexcept -> const_iterator
{
  return data_.cbegin();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::end() noexcept -> iterator
{
  return data_.end();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::end() const noexcept -> const_iterator
{
  return data_.end();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::cend() const noexcept -> const_iterator
{
  return data_.cend();
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::operator[](const uint index) noexcept
    -> reference
{
  return get(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::operator[](const uint index) const noexcept
    -> const_reference
{
  return get(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::data() noexcept -> ArrayType&
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::data() const noexcept -> const ArrayType&
{
  return data_;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::get(const uint index) noexcept
    -> reference
{
  return data_[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::get(const uint index) const noexcept
    -> const_reference
{
  return data_[index];
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::size() const noexcept -> size_type
{
  return kN - 1;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::toCString() const noexcept
    -> const_pointer
{
  return data_.data();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
std::basic_string<CharType> BasicString<CharType, kN>::toStdString()
    const noexcept
{
  auto str_view = toStdStringView();
  return std::basic_string<CharType>{str_view.data(), str_view.size()};
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
std::basic_string_view<CharType> BasicString<CharType, kN>::toStdStringView()
    const noexcept
{
  std::basic_string_view<CharType> str_view{data_.data(), data_.size()};
  return str_view;
}

/*!
  \details No detailed description

  \tparam indices No description.
  \param [in] other No description.
  \return No description
  */
template <typename CharType, uint kN> template <std::size_t ...indices> inline
constexpr auto BasicString<CharType, kN>::makeArray(
    const CharType (&other)[kN],
    std::index_sequence<indices...>) noexcept -> ArrayType
{
  const ArrayType a{{other[indices]...}};
  return a;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename CharType, uint kN> inline
constexpr auto BasicString<CharType, kN>::makeEmptyArray() noexcept -> ArrayType
{
  ArrayType array{};
  for (uint i = 0; i < kN; ++i)
    array[i] = '\0';
  return array;
}

/*!
  \details No detailed description

  \param [in] s No description.
  \return No description
  */
inline
constexpr std::size_t getSize(const char* s) noexcept
{
  std::size_t size = 0;
  for (; s[size] != '\0'; ++size);
  return size;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN1, uint kN2> inline
constexpr BasicString<CharType, kN1 + kN2 - 1> operator+(
    const BasicString<CharType, kN1>& lhs,
    const BasicString<CharType, kN2>& rhs) noexcept
{
  const auto result = concatenate(lhs, rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN1, uint kN2> inline
constexpr BasicString<CharType, kN1 + kN2 - 1> operator+(
    const CharType (&lhs)[kN1],
    const BasicString<CharType, kN2>& rhs) noexcept
{
  const auto result = concatenate(lhs, rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN1, uint kN2> inline
constexpr BasicString<CharType, kN1 + kN2 - 1> operator+(
    const BasicString<CharType, kN1>& lhs,
    const CharType (&rhs)[kN2]) noexcept
{
  const auto result = concatenate(lhs, rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN1, uint kN2> inline
constexpr bool operator==(
    const BasicString<CharType, kN1>& lhs,
    const BasicString<CharType, kN2>& rhs) noexcept
{
  bool result = lhs.size() == rhs.size();
  for (uint i = 0; result && (i < lhs.size()); ++i)
    result = lhs[i] == rhs[i];
  return result;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr bool operator==(
    const CharType* lhs,
    const BasicString<CharType, kN>& rhs) noexcept
{
  const std::basic_string_view<CharType> l{lhs};
  return l == rhs;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr bool operator==(
    const BasicString<CharType, kN>& lhs,
    const CharType* rhs) noexcept
{
  return rhs == lhs;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr bool operator==(
    const std::basic_string_view<CharType>& lhs,
    const BasicString<CharType, kN>& rhs) noexcept
{
  bool result = lhs.size() == rhs.size();
  for (uint i = 0; result && (i < rhs.size()); ++i)
    result = lhs[i] == rhs[i];
  return result;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr bool operator==(
    const BasicString<CharType, kN>& lhs,
    const std::basic_string_view<CharType>& rhs) noexcept
{
  return rhs == lhs;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN1, uint kN2> inline
constexpr bool operator!=(
    const BasicString<CharType, kN1>& lhs,
    const BasicString<CharType, kN2>& rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr bool operator!=(
    const CharType* lhs,
    const BasicString<CharType, kN>& rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr bool operator!=(
    const BasicString<CharType, kN>& lhs,
    const CharType* rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN1, uint kN2> inline
constexpr BasicString<CharType, kN1 + kN2 - 1> concatenate(
    const BasicString<CharType, kN1>& lhs,
    const BasicString<CharType, kN2>& rhs) noexcept
{
  constexpr uint size = kN1 + kN2 - 1;
  BasicString<CharType, size> result{};
  for (uint i = 0; i < lhs.size(); ++i)
    result[i] = lhs[i];
  for (uint i = 0; i < rhs.size(); ++i)
    result[i + lhs.size()] = rhs[i];
  return result;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN1, uint kN2> inline
constexpr BasicString<CharType, kN1 + kN2 - 1> concatenate(
    const CharType (&lhs)[kN1],
    const BasicString<CharType, kN2>& rhs) noexcept
{
  const auto l = toString(lhs);
  const auto result = concatenate(l, rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename CharType, uint kN1, uint kN2> inline
constexpr BasicString<CharType, kN1 + kN2 - 1> concatenate(
    const BasicString<CharType, kN1>& lhs,
    const CharType (&rhs)[kN2]) noexcept 
{
  const auto r = toString(rhs);
  const auto result = concatenate(lhs, r);
  return result;
}

/*!
  \details No detailed description

  \tparam CharType No description.
  \tparam kN No description.
  \param [in] other No description.
  \return No description
  */
template <typename CharType, uint kN> inline
constexpr BasicString<CharType, kN> toString(const CharType (&other)[kN]) noexcept
{
  const BasicString<CharType, kN> s{other};
  return s;
}

} // namespace zisc

#endif // _Z_STRING_INL_HPP__
