/*!
  \file constant_string-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONSTANT_STRING_INL_HPP
#define ZISC_CONSTANT_STRING_INL_HPP

#include "constant_string.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <string_view>
#include <utility>
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <Character Char, uint kN> inline
constexpr BasicConstantString<Char, kN>::BasicConstantString() noexcept
    : data_{makeEmptyArray()}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <Character Char, uint kN> inline
constexpr BasicConstantString<Char, kN>::BasicConstantString(
    const value_type (&other)[kN]) noexcept
        : data_{makeArray(other, std::make_index_sequence<kN>())}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::begin() noexcept -> iterator
{
  return data_.begin();
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::begin() const noexcept -> const_iterator
{
  return data_.begin();
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::cbegin() const noexcept -> const_iterator
{
  return data_.cbegin();
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::end() noexcept -> iterator
{
  return data_.end();
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::end() const noexcept -> const_iterator
{
  return data_.end();
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::cend() const noexcept -> const_iterator
{
  return data_.cend();
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::operator[](const uint index) noexcept
    -> reference
{
  return get(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::operator[](const uint index) const noexcept
    -> const_reference
{
  return get(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::data() noexcept -> ArrayType&
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::data() const noexcept -> const ArrayType&
{
  return data_;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::get(const uint index) noexcept
    -> reference
{
  return data_[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::get(const uint index) const noexcept
    -> const_reference
{
  return data_[index];
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::size() const noexcept -> size_type
{
  return kN - 1;
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
auto BasicConstantString<Char, kN>::toCStr() const noexcept -> const_pointer
{
  return data_.data();
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
auto BasicConstantString<Char, kN>::toStdStr() const noexcept
    -> std::basic_string_view<value_type>
{
  std::basic_string_view<value_type> str_view{data_.data(), data_.size()};
  return str_view;
}

/*!
  \details No detailed description

  \tparam indices No description.
  \param [in] other No description.
  \return No description
  */
template <Character Char, uint kN> template <std::size_t ...indices> inline
constexpr auto BasicConstantString<Char, kN>::makeArray(
    const value_type (&other)[kN],
    [[maybe_unused]] std::index_sequence<indices...> idx) noexcept -> ArrayType
{
  const ArrayType data{{other[indices]...}};
  return data;
}

/*!
  \details No detailed description

  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto BasicConstantString<Char, kN>::makeEmptyArray() noexcept -> ArrayType
{
  ArrayType data{};
  for (uint i = 0; i < kN; ++i)
    data[i] = cast<value_type>('\0');
  return data;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN1, uint kN2> inline
constexpr auto operator+(const BasicConstantString<Char, kN1>& lhs,
                         const BasicConstantString<Char, kN2>& rhs) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>
{
  const BasicConstantString<Char, kN1 + kN2 - 1> result = concatenate(lhs, rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN1, uint kN2> inline
constexpr auto operator+(const Char (&lhs)[kN1],
                         const BasicConstantString<Char, kN2>& rhs) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>
{
  const BasicConstantString<Char, kN1 + kN2 - 1> result = concatenate(lhs, rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN1, uint kN2> inline
constexpr auto operator+(const BasicConstantString<Char, kN1>& lhs,
                         const Char (&rhs)[kN2]) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>
{
  const BasicConstantString<Char, kN1 + kN2 - 1> result = concatenate(lhs, rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN1, uint kN2> inline
constexpr auto operator==(const BasicConstantString<Char, kN1>& lhs,
                          const BasicConstantString<Char, kN2>& rhs) noexcept -> bool
{
  bool result = lhs.size() == rhs.size();
  for (uint i = 0; result && (i < lhs.size()); ++i)
    result = lhs[i] == rhs[i];
  return result;
}

/*!
  \details No detailed description

  \tparam Char description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto operator==(const Char* lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept -> bool
{
  const std::basic_string_view<Char> l{lhs};
  return l == rhs;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto operator==(const BasicConstantString<Char, kN>& lhs,
                          const Char* rhs) noexcept -> bool
{
  return rhs == lhs;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto operator==(const std::basic_string_view<Char>& lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept -> bool
{
  bool result = lhs.size() == rhs.size();
  for (uint i = 0; result && (i < rhs.size()); ++i)
    result = lhs[i] == rhs[i];
  return result;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto operator==(const BasicConstantString<Char, kN>& lhs,
                          const std::basic_string_view<Char>& rhs) noexcept -> bool
{
  return rhs == lhs;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN1, uint kN2> inline
constexpr auto operator!=(const BasicConstantString<Char, kN1>& lhs,
                          const BasicConstantString<Char, kN2>& rhs) noexcept -> bool
{
  return !(lhs == rhs);
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto operator!=(const Char* lhs,
                          const BasicConstantString<Char, kN>& rhs) noexcept -> bool
{
  return !(lhs == rhs);
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto operator!=(const BasicConstantString<Char, kN>& lhs,
                          const Char* rhs) noexcept -> bool
{
  return !(lhs == rhs);
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN1, uint kN2> inline
constexpr auto concatenate(const BasicConstantString<Char, kN1>& lhs,
                           const BasicConstantString<Char, kN2>& rhs) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>
{
  constexpr uint size = kN1 + kN2 - 1;
  BasicConstantString<Char, size> result{};
  for (uint i = 0; i < lhs.size(); ++i)
    result[i] = lhs[i];
  for (uint i = 0; i < rhs.size(); ++i)
    result[i + lhs.size()] = rhs[i];
  return result;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN1, uint kN2> inline
constexpr auto concatenate(const Char (&lhs)[kN1],
                           const BasicConstantString<Char, kN2>& rhs) noexcept
    -> BasicConstantString<Char, kN1 + kN2 - 1>
{
  const BasicConstantString<Char, kN1> l = toString(lhs);
  const BasicConstantString<Char, kN1 + kN2 - 1> result = concatenate(l, rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN1 No description.
  \tparam kN2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Character Char, uint kN1, uint kN2> inline
constexpr auto concatenate(const BasicConstantString<Char, kN1>& lhs,
                           const Char (&rhs)[kN2]) noexcept 
    -> BasicConstantString<Char, kN1 + kN2 - 1>
{
  const BasicConstantString<Char, kN2> r = toString(rhs);
  const BasicConstantString<Char, kN1 + kN2 - 1> result = concatenate(lhs, r);
  return result;
}

/*!
  \details No detailed description

  \tparam Char No description.
  \tparam kN No description.
  \param [in] other No description.
  \return No description
  */
template <Character Char, uint kN> inline
constexpr auto toString(const Char (&other)[kN]) noexcept -> BasicConstantString<Char, kN>
{
  const BasicConstantString<Char, kN> s{other};
  return s;
}

} // namespace zisc

#endif // _Z_CONSTANT_STRING_INL_HPP__
