/*!
  \file point-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_POINT_INL_HPP
#define ZISC_POINT_INL_HPP

#include "point.hpp"
// Standard C++ library
#include <initializer_list>
#include <utility>
// Zisc
#include "arith_array.hpp"
#include "dimension.hpp"
#include "vector.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Point<Arith, kN>::Point() noexcept
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Point<Arith, kN>::Point(std::initializer_list<Arith> init_list) noexcept :
    Dimension<Arith, kN>(init_list)
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Point<Arith, kN>::Point(const ArrayType& other) noexcept :
    Dimension<Arith, kN>(other)
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Point<Arith, kN>::Point(ArrayType&& other) noexcept :
    Dimension<Arith, kN>(std::move(other))
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN> operator-(const Point<Arith, kN>& lhs,
                                      const Point<Arith, kN>& rhs) noexcept
{
  return Vector<Arith, kN>{lhs.data() - rhs.data()};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Point<Arith, kN> operator+(const Point<Arith, kN>& lhs,
                                     const Vector<Arith, kN>& rhs) noexcept
{
  return Point<Arith, kN>{lhs.data() + rhs.data()};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Point<Arith, kN> operator+(const Vector<Arith, kN>& lhs,
                                     const Point<Arith, kN>& rhs) noexcept
{
  return rhs + lhs;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Point<Arith, kN> operator-(const Point<Arith, kN>& lhs,
                                     const Vector<Arith, kN>& rhs) noexcept
{
  return Point<Arith, kN>{lhs.data() - rhs.data()};
}

} // namespace zisc

#endif // ZISC_POINT_INL_HPP
