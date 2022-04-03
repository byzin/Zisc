/*!
  \file algorithm.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALGORITHM_HPP
#define ZISC_ALGORITHM_HPP

// Standard C++ library
#include <concepts>
// Zisc
#include "concepts.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class Algorithm
{
 public:
  //! Return the absolute value of the given value
  template <Arithmetic Arith>
  static constexpr Arith abs(const Arith x) noexcept;

  //! Clamp the value \a x between \a lower and \a upper
  template <typename Type, typename LowerType, typename UpperType>
  static constexpr std::common_type_t<Type, LowerType, UpperType> clamp(
      const Type value, 
      const LowerType lower, 
      const UpperType upper) noexcept;

  //! Return a inverse number of the given value
  template <std::floating_point Float>
  static constexpr Float invert(const Float x) noexcept;

  //! Test whether a value is in a range
  template <bool kIsLeftClosed = true,
            bool kIsRightCloed = false,
            typename Type,
            typename LowerType,
            typename UpperType>
  static constexpr bool isInBounds(const Type& value, 
                                   const LowerType& lower, 
                                   const UpperType& upper) noexcept;

  //! Test whether a value is in a range: (\a lower, \a upper )
  template <typename Type, typename LowerType, typename UpperType>
  static constexpr bool isInOpenBounds(const Type& value, 
                                       const LowerType& lower, 
                                       const UpperType& upper) noexcept;

  //! Test whether a value is in a range: [\a lower, \a upper ]
  template <typename Type, typename LowerType, typename UpperType>
  static constexpr bool isInClosedBounds(const Type& value, 
                                         const LowerType& lower, 
                                         const UpperType& upper) noexcept;

  //! Test whether the given number is negative
  template <Arithmetic Arith>
  static constexpr bool isNegative(const Arith x) noexcept;

  //! Return the greater of two elements
  template <typename Type1, typename Type2>
  static constexpr const std::common_type_t<Type1, Type2>& max(
      const Type1& a,
      const Type2& b) noexcept;

  //! Return the smaller of two elements
  template <typename Type1, typename Type2>
  static constexpr const std::common_type_t<Type1, Type2>& min(
      const Type1& a,
      const Type2& b) noexcept;

  // Integer

  //! Return the sub-range of the given index 
  template <Integer Int1, Integer Int2, Integer Int3>
  static constexpr std::array<std::common_type_t<Int1, Int2, Int3>, 2> divideRange(
      const Int1 range,
      const Int2 num_of_division,
      const Int3 index) noexcept;

  //! Test whether the given value is odd
  template <Integer Int>
  static constexpr bool isOdd(const Int x) noexcept;
};

// STL style function aliases

//! Return the absolute value of the given value
template <Arithmetic Arith>
constexpr Arith abs(const Arith x) noexcept;

//! Clamp the value x to be between min and max
template <typename Type, typename LowerType, typename UpperType>
constexpr std::common_type_t<Type, LowerType, UpperType> clamp(
    const Type value, 
    const LowerType lower, 
    const UpperType upper) noexcept;

//! Return a inverse number
template <std::floating_point Float>
constexpr Float invert(const Float x) noexcept;

//! Return the large of two elements
template <typename Type1, typename Type2>
constexpr const std::common_type_t<Type1, Type2>& max(const Type1& a,
                                                      const Type2& b) noexcept;

//! Return the small of two elements
template <typename Type1, typename Type2>
constexpr const std::common_type_t<Type1, Type2>& min(const Type1& a,
                                                      const Type2& b) noexcept;

//! Test whether the given number is negative
template <Arithmetic Arith>
constexpr bool signbit(const Arith x) noexcept;

//

//! Test whether a value is in a range
template <bool kIsLeftClosed = true,
          bool kIsRightCloed = false,
          typename Type,
          typename LowerType,
          typename UpperType>
constexpr bool isInBounds(const Type& value, 
                          const LowerType& lower, 
                          const UpperType& upper) noexcept;

//! Test whether a value is in a range: (lower, upper)
template <typename Type, typename LowerType, typename UpperType>
constexpr bool isInOpenBounds(const Type& value, 
                              const LowerType& lower, 
                              const UpperType& upper) noexcept;

//! Test whether a value is in a range: [lower, upper]
template <typename Type, typename LowerType, typename UpperType>
constexpr bool isInClosedBounds(const Type& value, 
                                const LowerType& lower, 
                                const UpperType& upper) noexcept;

//! Return the sub-range of the given index 
template <Integer Int1, Integer Int2, Integer Int3>
constexpr std::array<std::common_type_t<Int1, Int2, Int3>, 2> divideRange(
    const Int1 range,
    const Int2 num_of_division,
    const Int3 index) noexcept;

//! Test whether the n is odd
template <Integer Int>
constexpr bool isOdd(const Int x) noexcept;

} // namespace zisc

/*!
  \example algorithm_example.cpp

  This is an example of how to use zisc::Algorithm.
  */

#include "algorithm-inl.hpp"

#endif // ZISC_ALGORITHM_HPP
