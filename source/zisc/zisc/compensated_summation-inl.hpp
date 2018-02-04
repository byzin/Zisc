/*!
  \file compensated_summation-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_COMPENSATED_SUMMATION_INL_HPP
#define ZISC_COMPENSATED_SUMMATION_INL_HPP

#include "compensated_summation.hpp"
// Standard C++ library
#include <initializer_list>
// Zisc
#include "math.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Float> inline
constexpr CompensatedSummation<Float>::CompensatedSummation() noexcept :
    sum_{cast<Float>(0.0)},
    compensation_{cast<Float>(0.0)}
{
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
constexpr CompensatedSummation<Float>::CompensatedSummation(const Float value)
    noexcept :
        sum_{value},
        compensation_{cast<Float>(0.0)}
{
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
constexpr CompensatedSummation<Float>::CompensatedSummation(
    std::initializer_list<Float> init_list) noexcept :
        sum_{*std::begin(init_list)},
        compensation_{cast<Float>(0.0)}
{
  add(init_list.begin() + 1, init_list.end());
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
constexpr Float CompensatedSummation<Float>::get() const noexcept
{
  return sum_ + compensation_;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
constexpr void CompensatedSummation<Float>::set(const Float value) noexcept
{
  sum_ = value;
  compensation_ = cast<Float>(0.0);
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
constexpr void CompensatedSummation<Float>::add(const Float value) noexcept
{
  const Float t = sum_ + value;
  compensation_ += (zisc::abs(value) < zisc::abs(sum_))
      ? (sum_ - t) + value
      : (value - t) + sum_;
  sum_ = t;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
constexpr void CompensatedSummation<Float>::add(
    std::initializer_list<Float> init_list) noexcept
{
  add(init_list.begin(), init_list.end());
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
constexpr void CompensatedSummation<Float>::add(
    typename std::initializer_list<Float>::const_iterator begin,
    typename std::initializer_list<Float>::const_iterator end) noexcept
{
  for (auto i = begin; i != end; ++i)
    add(*i);
}

} // namespace zisc

#endif // ZISC_COMPENSATED_SUMMATION_INL_HPP
