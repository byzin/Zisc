/*!
  \file vector-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_VECTOR_INL_HPP
#define ZISC_VECTOR_INL_HPP

#include "vector.hpp"
// Standard C++ library
#include <initializer_list>
#include <utility>
// Zisc
#include "arith_array.hpp"
#include "math.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN>::Vector() noexcept
{
}

/*!
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN>::Vector(std::initializer_list<Arith> init_list)
    noexcept :
        Dimension<Arith, kN>(init_list)
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN>::Vector(const ArrayType& other) noexcept :
    Dimension<Arith, kN>(other)
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN>::Vector(ArrayType&& other) noexcept :
    Dimension<Arith, kN>(std::move(other))
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr auto Vector<Arith, kN>::operator+=(const Vector& other) noexcept
    -> Vector&
{
  *this = *this + other;
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr auto Vector<Arith, kN>::operator-=(const Vector& other) noexcept
    -> Vector&
{
  *this = *this - other;
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr auto Vector<Arith, kN>::operator*=(const Arith scalar) noexcept
    -> Vector&
{
  *this = *this * scalar;
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr auto Vector<Arith, kN>::operator/=(const Arith scalar) noexcept
    -> Vector&
{
  *this = *this / scalar;
  return *this;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
Arith Vector<Arith, kN>::inverseNorm() const noexcept
{
  return cast<Arith>(1.0) / sqrt(squareNorm());
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
Arith Vector<Arith, kN>::norm() const noexcept
{
  return sqrt(squareNorm());
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
auto Vector<Arith, kN>::normalized() const noexcept -> Vector
{
  return *this * inverseNorm();
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Arith Vector<Arith, kN>::squareNorm() const noexcept
{
  return dot(*this, *this);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN> operator-(const Vector<Arith, kN>& vector) noexcept
{
  return Vector<Arith, kN>{-vector.data()};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN> operator+(const Vector<Arith, kN>& lhs,
                                      const Vector<Arith, kN>& rhs) noexcept
{
  return Vector<Arith, kN>{lhs.data() + rhs.data()};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN> operator-(const Vector<Arith, kN>& lhs,
                                      const Vector<Arith, kN>& rhs) noexcept
{
  return Vector<Arith, kN>{lhs.data() - rhs.data()};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN> operator*(const Vector<Arith, kN>& lhs,
                                      const Arith rhs) noexcept
{
  return Vector<Arith, kN>{lhs.data() * rhs};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN> operator*(const Arith lhs,
                                      const Vector<Arith, kN>& rhs) noexcept
{
  return rhs * lhs;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN> operator/(const Arith lhs,
                                      const Vector<Arith, kN>& rhs) noexcept
{
  return Vector<Arith, kN>{lhs / rhs.data()};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Vector<Arith, kN> operator/(const Vector<Arith, kN>& lhs,
                                      const Arith rhs) noexcept
{
  return Vector<Arith, kN>{lhs.data() / rhs};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr bool operator==(const Vector<Arith, kN>& lhs,
                          const Vector<Arith, kN>& rhs) noexcept
{
  return (lhs.data() == rhs.data());
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr bool operator!=(const Vector<Arith, kN>& lhs,
                          const Vector<Arith, kN>& rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Arith dot(const Vector<Arith, kN>& lhs,
                    const Vector<Arith, kN>& rhs) noexcept
{
  return dot(lhs.data(), rhs.data());
}

/*!
  */
template <typename Arith> inline
constexpr Vector<Arith, 3> cross(const Vector<Arith, 3>& lhs,
                                 const Vector<Arith, 3>& rhs) noexcept
{
  return Vector<Arith, 3>{cross(lhs.data(), rhs.data())};
}

} // namespace zisc

#endif // ZISC_VECTOR_INL_HPP
