/*!
  \file arith_array-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  No copyright
  */

#ifndef ZISC_ARITH_ARRAY_INL_HPP
#define ZISC_ARITH_ARRAY_INL_HPP

#include "arith_array.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>
// Zisc
#include "compensated_summation.hpp"
#include "math.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN>::ArithArray() noexcept : data_{}
{
  constexpr auto value = cast<Arith>(0);
  fill(value);
}

/*!
  \details No detailed description

  \param [in] value_list No description.
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN>::ArithArray(
    const std::initializer_list<Arith> value_list) noexcept :
        data_{makeArray(value_list, std::make_index_sequence<kN>())}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN>::ArithArray(const ArrayType& other) noexcept :
    data_{other}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN>::ArithArray(ArrayType&& other) noexcept :
    data_{std::move(other.data_)}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::begin() noexcept -> iterator
{
  return data_.begin();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::begin() const noexcept -> const_iterator
{
  return data_.begin();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::cbegin() const noexcept -> const_iterator
{
  return data_.cbegin();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::end() noexcept -> iterator
{
  return data_.end();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::end() const noexcept -> const_iterator
{
  return data_.end();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::cend() const noexcept -> const_iterator
{
  return data_.cend();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::operator+=(const ArithArray& other) noexcept
    -> ArithArray&
{
  *this = *this + other;
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::operator-=(const ArithArray& other) noexcept
    -> ArithArray&
{
  *this = *this - other;
  return *this;
}

/*!
  \details No detailed description

  \param [in] scalar No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::operator*=(const Arith scalar) noexcept
    -> ArithArray&
{
  *this = *this * scalar;
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::operator*=(const ArithArray& other) noexcept
    -> ArithArray&
{
  *this = *this * other;
  return *this;
}

/*!
  \details No detailed description

  \param [in] scalar No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::operator/=(const Arith scalar) noexcept
    -> ArithArray&
{
  *this = *this / scalar;
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::operator/=(const ArithArray& other) noexcept
    -> ArithArray&
{
  *this = *this / other;
  return *this;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::operator[](const uint index) noexcept
    -> reference
{
  return get(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::operator[](const uint index) const noexcept
    -> const_reference
{
  return get(index);
}

/*!
  \details No detailed description

  \param [in] lower No description.
  \param [in] upper No description.
  */
template <typename Arith, uint kN> inline
constexpr void ArithArray<Arith, kN>::clampAll(const Arith lower,
                                               const Arith upper) noexcept
{
  for (auto& element : *this)
    element = zisc::clamp(element, lower, upper);
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <typename Arith, uint kN> inline
constexpr void ArithArray<Arith, kN>::fill(const Arith& value) noexcept
{
  for (uint index = 0; index < size(); ++index)
    get(index) = value;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::get(const uint index) noexcept
    -> reference
{
  return data_[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::get(const uint index) const noexcept
    -> const_reference
{
  return data_[index];
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool ArithArray<Arith, kN>::hasInf() const noexcept
{
  bool result = false;
  if constexpr (kIsFloat<Arith>) {
    for (uint index = 0; !result && (index < size()); ++index)
      result = isInf(get(index));
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool ArithArray<Arith, kN>::hasNan() const noexcept
{
  bool result = false;
  if constexpr (kIsFloat<Arith>) {
    for (uint index = 0; !result && (index < size()); ++index)
      result = isNan(get(index));
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool ArithArray<Arith, kN>::hasSubnormal() const noexcept
{
  bool result = false;
  if constexpr (kIsFloat<Arith>) {
    for (uint index = 0; !result && (index < size()); ++index)
      result = isSubnormal(get(index));
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool ArithArray<Arith, kN>::hasValue(const Arith value) const noexcept
{
  bool result = false;
  for (uint index = 0; !result && (index < size()); ++index)
    result = get(index) == value;
  return result;
}

/*!
  \details No detailed description

  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool ArithArray<Arith, kN>::isAllInBounds(
    const Arith lower,
    const Arith upper) const noexcept
{
  bool result = true;
  for (uint index = 0; result && (index < size()); ++index)
    result = isInBounds(get(index), lower, upper);
  return result;
}

/*!
  \details No detailed description

  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool ArithArray<Arith, kN>::isAllInClosedBounds(
    const Arith lower,
    const Arith upper) const noexcept
{
  bool result = true;
  for (uint index = 0; result && (index < size()); ++index)
    result = isInClosedBounds(get(index), lower, upper);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool ArithArray<Arith, kN>::isAllZero() const noexcept
{
  bool result = true;
  for (uint index = 0; result && (index < size()); ++index) {
    constexpr auto zero = cast<Arith>(0);
    result = get(index) == zero;
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr Arith ArithArray<Arith, kN>::max() const noexcept
{
  Arith m = get(0);
  for (uint index = 1; index < size(); ++index)
    m = zisc::max(get(index), m);
  return m;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr Arith ArithArray<Arith, kN>::min() const noexcept
{
  Arith m = get(0);
  for (uint index = 1; index < size(); ++index)
    m = zisc::min(get(index), m);
  return m;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] value No description.
  */
template <typename Arith, uint kN> inline
constexpr void ArithArray<Arith, kN>::set(const uint index,
                                          const Arith value) noexcept
{
  data_[index] = value;
}

/*!
  \details No detailed description

  \param [in] value_list No description.
  */
template <typename Arith, uint kN> inline
constexpr void ArithArray<Arith, kN>::set(std::initializer_list<Arith> value_list)
    noexcept
{
  data_ = makeArray(value_list, std::make_index_sequence<kN>());
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr auto ArithArray<Arith, kN>::size() noexcept -> size_type
{
  return cast<size_type>(kN);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kN> inline
constexpr Arith ArithArray<Arith, kN>::sum() const noexcept
{
  if constexpr (kIsFloat<Arith>) {
    CompensatedSummation<Arith> sum{get(0)};
    for (uint index = 1; index < size(); ++index)
      sum.add(get(index));
    return sum.get();
  }
  else {
    Arith sum = get(0);
    for (uint index = 1; index < size(); ++index)
      sum += get(index);
    return sum;
  }
}

/*!
  \details No detailed description

  \tparam indices No description.
  \param [in] value_list No description.
  \return No description
  */
template <typename Arith, uint kN> template <std::size_t ...indices> inline
constexpr auto ArithArray<Arith, kN>::makeArray(
    const std::initializer_list<Arith> value_list,
    const std::index_sequence<indices...>) noexcept -> ArrayType
{
  static_assert(sizeof...(indices) == kN);
  const ArrayType a{{*(value_list.begin() + indices)...}};
  return a;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] array No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator-(const ArithArray<Arith, kN>& array) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = -array[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator+(const ArithArray<Arith, kN>& lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = lhs[index] + rhs[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator-(const ArithArray<Arith, kN>& lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = lhs[index] - rhs[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator*(const ArithArray<Arith, kN>& lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = lhs[index] * rhs[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator*(const Arith lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = lhs * rhs[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator*(const ArithArray<Arith, kN>& lhs,
                                          const Arith rhs) noexcept
{
  return rhs * lhs;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator/(const ArithArray<Arith, kN>& lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = lhs[index] / rhs[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator/(const Arith lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = lhs / rhs[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> operator/(const ArithArray<Arith, kN>& lhs,
                                          const Arith rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = lhs[index] / rhs;
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool operator==(const ArithArray<Arith, kN>& lhs,
                          const ArithArray<Arith, kN>& rhs) noexcept
{
  bool result = true;
  for (uint index = 0; result && (index < kN); ++index)
    result = lhs[index] == rhs[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr bool operator!=(const ArithArray<Arith, kN>& lhs,
                          const ArithArray<Arith, kN>& rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr Arith dot(const ArithArray<Arith, kN>& lhs,
                    const ArithArray<Arith, kN>& rhs) noexcept
{
  Arith result = cast<Arith>(0);
  for (uint index = 0; index < kN; ++index)
    result += lhs[index] * rhs[index];
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith> inline
constexpr ArithArray<Arith, 3>  cross(const ArithArray<Arith, 3>& lhs,
                                      const ArithArray<Arith, 3>& rhs) noexcept
{
  return ArithArray<Arith, 3>{lhs[1] * rhs[2] - lhs[2] * rhs[1],
                              lhs[2] * rhs[0] - lhs[0] * rhs[2],
                              lhs[0] * rhs[1] - lhs[1] * rhs[0]};
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> maxElements(
    const ArithArray<Arith, kN>& lhs,
    const ArithArray<Arith, kN>& rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = max(lhs[index], rhs[index]);
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kN> inline
constexpr ArithArray<Arith, kN> minElements(
    const ArithArray<Arith, kN>& lhs,
    const ArithArray<Arith, kN>& rhs) noexcept
{
  ArithArray<Arith, kN> result;
  for (uint index = 0; index < result.size(); ++index)
    result[index] = min(lhs[index], rhs[index]);
  return result;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  */
template <typename Arith, uint kN> inline
constexpr void minMaxElements(ArithArray<Arith, kN>& lhs,
                              ArithArray<Arith, kN>& rhs) noexcept
{
  for (uint index = 0; index < kN; ++index) {
    if (rhs[index] < lhs[index])
      swapElement(lhs, rhs, index);
  }
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kN No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \param [in] index No description.
  */
template <typename Arith, uint kN> inline
constexpr void swapElement(const ArithArray<Arith, kN>& lhs,
                           const ArithArray<Arith, kN>& rhs,
                           const uint index) noexcept
{
  const Arith tmp = rhs[index];
  rhs[index] = lhs[index];
  lhs[index] = tmp;
}

} // namespace zisc

#endif // ZISC_ARITH_ARRAY_INL_HPP
