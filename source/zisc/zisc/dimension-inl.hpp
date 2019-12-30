/*!
  \file dimension-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_DIMENSION_INL_HPP
#define ZISC_DIMENSION_INL_HPP

#include "dimension.hpp"
// Standard C++ library
#include <initializer_list>
#include <utility>
// Zisc
#include "arith_array.hpp"
#include "math.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Dimension<Arith, kN>::Dimension() noexcept
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Dimension<Arith, kN>::Dimension(std::initializer_list<Arith> init_list)
    noexcept :
        data_{init_list}
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Dimension<Arith, kN>::Dimension(const ArrayType& other) noexcept :
    data_{other}
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Dimension<Arith, kN>::Dimension(ArrayType&& other) noexcept :
    data_{std::move(other)}
{
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Arith& Dimension<Arith, kN>::operator[](const uint index) noexcept
{
  return get(index);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr const Arith& Dimension<Arith, kN>::operator[](const uint index)
    const noexcept
{
  return get(index);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr auto Dimension<Arith, kN>::data() noexcept -> ArrayType&
{
  return data_;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr auto Dimension<Arith, kN>::data() const noexcept -> const ArrayType&
{
  return data_;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Arith& Dimension<Arith, kN>::get(const uint index) noexcept
{
  return data_[index];
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr const Arith& Dimension<Arith, kN>::get(const uint index) const noexcept
{
  return data_[index];
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr uint Dimension<Arith, kN>::size() noexcept
{
  return kN;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr void Dimension<Arith, kN>::set(const uint index,
                                         const Arith value) noexcept
{
  data_.set(index, value);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr void Dimension<Arith, kN>::set(
    std::initializer_list<Arith> init_list) noexcept
{
  data_.set(init_list);
}

} // namespace zisc

#endif // ZISC_DIMENSION_INL_HPP
