/*!
  \file arithmetic_array-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ARITHMETIC_ARRAY_INL_HPP
#define ZISC_ARITHMETIC_ARRAY_INL_HPP

#include "arithmetic_array.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <type_traits>
// Zisc
#include "compensated_summation.hpp"
#include "math.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr ArithmeticArray<Arithmetic, kN>::ArithmeticArray() noexcept :
    array_{}
{
  constexpr auto value = cast<Arithmetic>(0);
  fill(value);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> template <typename ...Types> inline
constexpr ArithmeticArray<Arithmetic, kN>::ArithmeticArray(
    const Types ...values) noexcept :
        array_{{cast<Arithmetic>(values)...}}
{
  constexpr auto num_of_arguments = sizeof...(Types);
  static_assert(num_of_arguments == size(), "The num of arguments isn't match kN.");
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
ArithmeticArray<Arithmetic, kN>::ArithmeticArray(
    const std::array<Arithmetic, kN>& other) noexcept
{
  ZISC_ASSERT(other.size() == kN, "The size of array is wrong.");
  for (uint i = 0; i < kN; ++i)
    get(i) = other[i];
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr ArithmeticArray<Arithmetic, kN>::ArithmeticArray(
    const ArithmeticArray& other) noexcept :
        array_{other.array_}
{
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::begin() noexcept -> iterator
{
  return array_.elements_;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::begin() const noexcept -> const_iterator
{
  return array_.elements_;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::cbegin() const noexcept -> const_iterator
{
  return array_.elements_;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::end() noexcept -> iterator
{
  return array_.elements_ + size();
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::end() const noexcept -> const_iterator
{
  return array_.elements_ + size();
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::cend() const noexcept -> const_iterator
{
  return array_.elements_ + size();
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator+(
    const ArithmeticArray& other) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index)
    array[index] = get(index) + other[index];
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator-(
    const ArithmeticArray& other) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index)
    array[index] = get(index) - other[index];
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator*(
    const Arithmetic scalar) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index)
    array[index] = get(index) * scalar;
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator*(
    const ArithmeticArray& other) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index)
    array[index] = get(index) * other[index];
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator/(
    const Arithmetic scalar) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index)
    array[index] = get(index) / scalar;
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator/(
    const ArithmeticArray& other) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index)
    array[index] = get(index) / other[index];
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator=(
    const ArithmeticArray& other) noexcept -> ArithmeticArray&
{
  for (uint index = 0; index < size(); ++index)
    get(index) = other[index];
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator+=(
    const ArithmeticArray& other) noexcept -> ArithmeticArray&
{
  for (uint index = 0; index < size(); ++index)
    get(index) += other[index];
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator-=(
    const ArithmeticArray& other) noexcept -> ArithmeticArray&
{
  for (uint index = 0; index < size(); ++index)
    get(index) -= other[index];
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator*=(
    const Arithmetic scalar) noexcept -> ArithmeticArray&
{
  for (uint index = 0; index < size(); ++index)
    get(index) *= scalar;
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator*=(
    const ArithmeticArray& other) noexcept -> ArithmeticArray&
{
  for (uint index = 0; index < size(); ++index)
    get(index) *= other[index];
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator/=(
    const Arithmetic scalar) noexcept -> ArithmeticArray&
{
  for (uint index = 0; index < size(); ++index)
    get(index) /= scalar;
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::operator/=(
    const ArithmeticArray& other) noexcept -> ArithmeticArray&
{
  for (uint index = 0; index < size(); ++index)
    get(index) /= other[index];
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr Arithmetic& ArithmeticArray<Arithmetic, kN>::operator[](
    const uint index) noexcept
{
  return get(index);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr const Arithmetic& ArithmeticArray<Arithmetic, kN>::operator[](
    const uint index) const noexcept
{
  return get(index);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr void ArithmeticArray<Arithmetic, kN>::clampAll(
    const Arithmetic min_value,
    const Arithmetic max_value) noexcept
{
  for (auto& element : *this)
    element = zisc::clamp(element, min_value, max_value);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::divideScalar(
    const Arithmetic scalar) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index) {
    array[index] = scalar / get(index);
  }
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr void ArithmeticArray<Arithmetic, kN>::fill(const Arithmetic value) noexcept
{
  for (uint index = 0; index < size(); ++index)
    get(index) = value;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr Arithmetic& ArithmeticArray<Arithmetic, kN>::get(const uint index) noexcept
{
  return array_.elements_[index];
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr const Arithmetic& ArithmeticArray<Arithmetic, kN>::get(const uint index) const noexcept
{
  return array_.elements_[index];
}

/*!
  */
template <typename Arithmetic, uint kN> inline
constexpr bool ArithmeticArray<Arithmetic, kN>::hasValue(const Arithmetic value) const noexcept
{
  bool result = false;
  for (uint index = 0; (index < size()) && !result; ++index)
    result = get(index) == value;
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr bool ArithmeticArray<Arithmetic, kN>::isAllInBounds(
    const Arithmetic lower,
    const Arithmetic upper) const noexcept
{
  bool result = true;
  for (uint index = 0; (index < size()) && result; ++index)
    result = isInBounds(get(index), lower, upper);
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr bool ArithmeticArray<Arithmetic, kN>::isAllInClosedBounds(
    const Arithmetic lower,
    const Arithmetic upper) const noexcept
{
  bool result = true;
  for (uint index = 0; (index < size()) && result; ++index)
    result = isInClosedBounds(get(index), lower, upper);
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr bool ArithmeticArray<Arithmetic, kN>::isAllZero() const noexcept
{
  bool result = true;
  for (uint index = 0; (index < size()) && result; ++index) {
    constexpr auto zero = cast<Arithmetic>(0);
    result = get(index) == zero;
  }
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr Arithmetic ArithmeticArray<Arithmetic, kN>::max() const noexcept
{
  Arithmetic max = get(0);
  for (uint index = 1; index < size(); ++index)
    max = zisc::max(get(index), max);
  return max;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr Arithmetic ArithmeticArray<Arithmetic, kN>::min() const noexcept
{
  Arithmetic min = get(0);
  for (uint index = 1; index < size(); ++index)
    min = zisc::min(get(index), min);
  return min;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::maxElements(
    const ArithmeticArray& other) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index)
    array[index] = zisc::max(other[index], get(index));
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr auto ArithmeticArray<Arithmetic, kN>::minElements(
    const ArithmeticArray& other) const noexcept -> ArithmeticArray
{
  ArithmeticArray array;
  for (uint index = 0; index < size(); ++index)
    array[index] = zisc::min(other[index], get(index));
  return array;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr uint ArithmeticArray<Arithmetic, kN>::size() noexcept
{
  return kN;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
void ArithmeticArray<Arithmetic, kN>::set(const uint index, 
                                          const Arithmetic value) noexcept
{
  ZISC_ASSERT(index < size(), "The index is out of range.");
  get(index) = value;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> template <typename ...Types> inline
void ArithmeticArray<Arithmetic, kN>::setElements(const Types ...values) noexcept
{
  constexpr auto num_of_arguments = sizeof...(Types);
  static_assert(num_of_arguments == size(), "The num of arguments isn't match kN.");
  setElements<0>(cast<Arithmetic>(values)...);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
Arithmetic ArithmeticArray<Arithmetic, kN>::sum() const noexcept
{
  return sumArray<Arithmetic>(array_);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN>
template <uint index, typename ...Types> inline
void ArithmeticArray<Arithmetic, kN>::setElements(const Arithmetic value, 
                                                  const Types ...values) noexcept
{
  setElements<index>(value);
  setElements<index + 1>(values...);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> template <uint index> inline
void ArithmeticArray<Arithmetic, kN>::setElements(const Arithmetic value) noexcept
{
  set(index, value);
}

template <typename Arithmetic, uint kN> template <typename Type> inline
Type ArithmeticArray<Arithmetic, kN>::sumArray(
    const Array& array,
    EnableIfInteger<Type>) noexcept
{
  Type sum = array.elements_[0];
  for (uint index = 1; index < kN; ++index)
    sum += array.elements_[index];
  return sum;
}

template <typename Arithmetic, uint kN> template <typename Type> inline
Type ArithmeticArray<Arithmetic, kN>::sumArray(
    const Array& array,
    EnableIfFloat<Type>) noexcept
{
  CompensatedSummation<Type> sum{array.elements_[0]};
  for (uint index = 1; index < kN; ++index)
    sum.add(array.elements_[index]);
  return sum.get();
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr bool operator==(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b) noexcept
{
  bool result = true;
  for (uint index = 0; (index < kN) && result; ++index)
    result = a[index] == b[index];
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr bool operator!=(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b) noexcept
{
  return !(a == b);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr ArithmeticArray<Arithmetic, kN> operator*(
    const Arithmetic scalar,
    const ArithmeticArray<Arithmetic, kN>& array) noexcept
{
  return array * scalar;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr ArithmeticArray<Arithmetic, kN> operator/(
    const Arithmetic scalar,
    const ArithmeticArray<Arithmetic, kN>& array) noexcept
{
  return array.divideScalar(scalar);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic> inline
constexpr ArithmeticArray<Arithmetic, 3>  cross(
    const ArithmeticArray<Arithmetic, 3>& a, 
    const ArithmeticArray<Arithmetic, 3>& b) noexcept
{
  return ArithmeticArray<Arithmetic, 3>{a[1] * b[2] - a[2] * b[1],
                                        a[2] * b[0] - a[0] * b[2],
                                        a[0] * b[1] - a[1] * b[0]};
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr Arithmetic dot(
    const ArithmeticArray<Arithmetic, kN>& a, 
    const ArithmeticArray<Arithmetic, kN>& b) noexcept
{
  Arithmetic dot = a[0] * b[0];
  for (uint index = 1; index < kN; ++index)
    dot += a[index] * b[index];
  return dot;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr ArithmeticArray<Arithmetic, kN> maxElements(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b) noexcept
{
  return a.maxElements(b);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr ArithmeticArray<Arithmetic, kN> minElements(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b) noexcept
{
  return a.minElements(b);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr void minMaxElements(
    ArithmeticArray<Arithmetic, kN>& a,
    ArithmeticArray<Arithmetic, kN>& b) noexcept
{
  for (uint i = 0; i < kN; ++i) {
    if (b[i] < a[i])
      swapElement(a, b, i);
  }
}

template <typename Arithmetic, uint kN> inline
constexpr void swapElement(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b,
    const uint index) noexcept
{
  const Arithmetic tmp = b[index];
  b[index] = a[index];
  a[index] = tmp;
}

} // namespace zisc

#endif // ZISC_ARITHMETIC_ARRAY_INL_HPP
