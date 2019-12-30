/*!
  \file vector.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_VECTOR_HPP
#define ZISC_VECTOR_HPP

// Standard C++ library
#include <initializer_list>
// Zisc
#include "arith_array.hpp"
#include "dimension.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Geometric vector
  \details
  No detailed.
  */
template <typename Arith, uint kN>
class Vector : public Dimension<Arith, kN>
{
 public:
  using ArrayType = typename Dimension<Arith, kN>::ArrayType;


  //! Initialize elements with 0
  constexpr Vector() noexcept;

  //! Initialize elements with values
  constexpr Vector(std::initializer_list<Arith> init_list) noexcept;

  //! Initialize
  constexpr Vector(const ArrayType& other) noexcept;

  //! Initialize
  constexpr Vector(ArrayType&& other) noexcept;


  //!
  constexpr Vector& operator+=(const Vector& other) noexcept;

  //!
  constexpr Vector& operator-=(const Vector& other) noexcept;

  //!
  constexpr Vector& operator*=(const Arith scalar) noexcept;

  //!
  constexpr Vector& operator/=(const Arith scalar) noexcept;


  //! Return the inverse norm
  Arith inverseNorm() const noexcept;

  //! Return the norm
  Arith norm() const noexcept;

  //! Return a normalized vector
  Vector normalized() const noexcept;

  //! Return the square norm
  constexpr Arith squareNorm() const noexcept;
};

//! Get a vector in the opposite direction
template <typename Arith, uint kN>
constexpr Vector<Arith, kN> operator-(const Vector<Arith, kN>& vector) noexcept;

//! Perform addition operation on two vectors
template <typename Arith, uint kN>
constexpr Vector<Arith, kN> operator+(const Vector<Arith, kN>& lhs,
                                      const Vector<Arith, kN>& rhs) noexcept;

//! Perform subtraction operation on two vectors
template <typename Arith, uint kN>
constexpr Vector<Arith, kN> operator-(const Vector<Arith, kN>& lhs,
                                      const Vector<Arith, kN>& rhs) noexcept;

//! Perform multiplication operation on a scalar and a vector
template <typename Arith, uint kN>
constexpr Vector<Arith, kN> operator*(const Arith lhs,
                                      const Vector<Arith, kN>& rhs) noexcept;

//! Perform multiplication operation on a scalar and a vector
template <typename Arith, uint kN>
constexpr Vector<Arith, kN> operator*(const Vector<Arith, kN>& lhs,
                                      const Arith rhs) noexcept;

//! Perform division operation on a scalar and a vector
template <typename Arith, uint kN>
constexpr Vector<Arith, kN> operator/(const Arith lhs,
                                      const Vector<Arith, kN>& rhs) noexcept;

//! Perform division operation on a scalar and a vector
template <typename Arith, uint kN>
constexpr Vector<Arith, kN> operator/(const Vector<Arith, kN>& lhs,
                                      const Arith rhs) noexcept;

//! Check if the two vectors are same
template <typename Arith, uint kN>
constexpr bool operator==(const Vector<Arith, kN>& lhs,
                          const Vector<Arith, kN>& rhs) noexcept;

//! Check if the two vectors are not same
template <typename Arith, uint kN>
constexpr bool operator!=(const Vector<Arith, kN>& lhs, 
                          const Vector<Arith, kN>& rhs) noexcept;

//! Calculate dot product
template <typename Arith, uint kN>
constexpr Arith dot(const Vector<Arith, kN>& lhs,
                    const Vector<Arith, kN>& rhs) noexcept;

//! Calculate cross product
template <typename Arith>
constexpr Vector<Arith, 3> cross(const Vector<Arith, 3>& lhs,
                                 const Vector<Arith, 3>& rhs) noexcept;

} // namespace zisc

#include "vector-inl.hpp"

#endif // ZISC_VECTOR_HPP
