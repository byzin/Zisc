/*!
  \file compensated_summation.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_COMPENSATED_SUMMATION_HPP
#define ZISC_COMPENSATED_SUMMATION_HPP

// Standard C++ library
#include <initializer_list>
#include <type_traits>
// Zisc
#include "type_traits.hpp"

namespace zisc {

/*!
  \details
  The details of this algorithm are explained in the following URL
  http://en.wikipedia.org/wiki/Kahan_summation_algorithm
  */
template <typename Float>
class CompensatedSummation
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

 public:
  //! Initialize with 0
  constexpr CompensatedSummation() noexcept;

  //! Initialize with the value
  constexpr CompensatedSummation(const Float value) noexcept;

  //! Initialize with the values
  constexpr CompensatedSummation(std::initializer_list<Float> init_list) noexcept;


  //! Return the value
  constexpr Float get() const noexcept;

  //! Initialize with the value
  constexpr void set(const Float value) noexcept;

  //! Sum value
  constexpr void add(const Float value) noexcept;

  //! Sum values
  constexpr void add(std::initializer_list<Float> init_list) noexcept;

 private:
  //! Sum values
  constexpr void add(
      typename std::initializer_list<Float>::const_iterator begin,
      typename std::initializer_list<Float>::const_iterator end) noexcept;


  Float sum_,
        compensation_;
};

} // namespace zisc

#include "compensated_summation-inl.hpp"

#endif // ZISC_COMPENSATED_SUMMATION_HPP
