/*!
  \file point.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_POINT_HPP
#define ZISC_POINT_HPP

// Standard C++ library
#include <initializer_list>
// Zisc
#include "arith_array.hpp"
#include "dimension.hpp"
#include "vector.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief N dimension point
  \details
  No detailed.
  */
template <typename Arith, uint kN>
class Point : public Dimension<Arith, kN>
{
 public:
  using ArrayType = typename Dimension<Arith, kN>::ArrayType;


  //! Initialize elements with 0
  constexpr Point() noexcept;

  //! Initialize elemens with values
  constexpr Point(std::initializer_list<Arith> init_list) noexcept;

  //! Initialize
  constexpr Point(const ArrayType& other) noexcept;

  //! Initialize
  constexpr Point(ArrayType&& other) noexcept;
};

//! Perform subtraction operation on two points
template <typename Arith, uint kN>
constexpr Vector<Arith, kN> operator-(const Point<Arith, kN>& lhs,
                                      const Point<Arith, kN>& rhs) noexcept;

//! Perform addition operation on a point and a vector
template <typename Arith, uint kN>
constexpr Point<Arith, kN> operator+(const Point<Arith, kN>& lhs,
                                     const Vector<Arith, kN>& rhs) noexcept;

//! Perform addition operation on a point and a vector
template <typename Arith, uint kN>
constexpr Point<Arith, kN> operator+(const Vector<Arith, kN>& lhs,
                                     const Point<Arith, kN>& rhs) noexcept;

//! Perform subtraction operation on a point and a vector
template <typename Arith, uint kN>
constexpr Point<Arith, kN> operator-(const Point<Arith, kN>& lhs,
                                     const Vector<Arith, kN>& rhs) noexcept;

} // namespace zisc

#include "point-inl.hpp"

#endif // ZISC_POINT_HPP
