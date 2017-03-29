/*!
  \file array-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ARRAY_INL_HPP
#define ZISC_ARRAY_INL_HPP

#include "array.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <utility>
// Zisc
#include "error.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr Array<Type, kN>::Array() noexcept :
    array_{}
{
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> template <typename ...Types> inline
constexpr Array<Type, kN>::Array(Types&& ...values) noexcept :
    array_{{std::forward<Types>(values)...}}
{
  constexpr auto num_of_arguments = sizeof...(Types);
  static_assert(num_of_arguments == size(), "The num of arguments isn't match kN.");
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
Array<Type, kN>::Array(const std::array<Type, kN>& other) noexcept :
    array_{}
{
  ZISC_ASSERT(other.size() == kN, "The size of array is wrong.");
  for (uint i = 0; i < kN; ++i)
    get(i) = other[i];
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
Array<Type, kN>::Array(std::array<Type, kN>&& other) noexcept :
    array_{}
{
  ZISC_ASSERT(other.size() == kN, "The size of array is wrong.");
  for (uint i = 0; i < kN; ++i)
    get(i) = std::move(other[i]);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr Array<Type, kN>::Array(const Array& other) noexcept :
        array_{other.array_}
{
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr Array<Type, kN>::Array(Array&& other) noexcept :
        array_{}
{
  for (uint i = 0; i < kN; ++i)
    get(i) = std::move(other[i]);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::begin() noexcept -> iterator
{
  return array_.elements_;
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::begin() const noexcept -> const_iterator
{
  return array_.elements_;
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::cbegin() const noexcept -> const_iterator
{
  return array_.elements_;
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::end() noexcept -> iterator
{
  return array_.elements_ + size();
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::end() const noexcept -> const_iterator
{
  return array_.elements_ + size();
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::cend() const noexcept -> const_iterator
{
  return array_.elements_ + size();
}

/*!
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::operator=(const Array& other) noexcept
    -> Array&
{
  array_ = other.array_;
  return *this;
}

/*!
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::operator=(Array&& other) noexcept
    -> Array&
{
  for (uint i = 0; i < kN; ++i)
    get(i) = std::move(other[i]);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr Type& Array<Type, kN>::operator[](const uint index) noexcept
{
  return get(index);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr const Type& Array<Type, kN>::operator[](const uint index) const noexcept
{
  return get(index);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr void Array<Type, kN>::fill(const Type& value) noexcept
{
  for (uint index = 0; index < size(); ++index)
    get(index) = value;
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr Type& Array<Type, kN>::get(const uint index) noexcept
{
  return array_.elements_[index];
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr const Type& Array<Type, kN>::get(const uint index) const noexcept
{
  return array_.elements_[index];
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
constexpr auto Array<Type, kN>::size() noexcept -> size_type
{
  return kN;
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
void Array<Type, kN>::set(const uint index, const Type& value) noexcept
{
  ZISC_ASSERT(index < size(), "The index is out of range.");
  get(index) = value;
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> inline
void Array<Type, kN>::set(const uint index, Type&& value) noexcept
{
  ZISC_ASSERT(index < size(), "The index is out of range.");
  get(index) = std::move(value);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> template <typename ...Types> inline
void Array<Type, kN>::setElements(Types&& ...values) noexcept
{
  constexpr auto num_of_arguments = sizeof...(Types);
  static_assert(num_of_arguments == size(), "The num of arguments isn't match kN.");
  setElements<0>(std::forward<Types>(values)...);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN>
template <uint index, typename ...Types> inline
void Array<Type, kN>::setElements(const Type& value, Types&& ...values) noexcept
{
  setElements<index>(value);
  setElements<index + 1>(std::forward<Types>(values)...);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN>
template <uint index, typename ...Types> inline
void Array<Type, kN>::setElements(Type&& value, Types&& ...values) noexcept
{
  setElements<index>(std::move(value));
  setElements<index + 1>(std::forward<Types>(values)...);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> template <uint index> inline
void Array<Type, kN>::setElements(const Type& value) noexcept
{
  set(index, value);
}

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN> template <uint index> inline
void Array<Type, kN>::setElements(Type&& value) noexcept
{
  set(index, std::move(value));
}

/*!
  */
template <typename Type, uint kN> inline
constexpr void swapElement(const Array<Type, kN>& a,
                           const Array<Type, kN>& b,
                           const uint index) noexcept
{
  const Type tmp = b[index];
  b[index] = a[index];
  a[index] = tmp;
}

} // namespace zisc

#endif // ZISC_ARRAY_INL_HPP
