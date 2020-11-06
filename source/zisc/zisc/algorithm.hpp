/*!
  \file algorithm.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ALGORITHM_HPP
#define ZISC_ALGORITHM_HPP

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
  template <FloatingPoint Float>
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

  //! Test whether the given value is negative
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
template <FloatingPoint Float>
constexpr Float invert(const Float x) noexcept;

//! Return the large of two elements
template <typename Type1, typename Type2>
constexpr const std::common_type_t<Type1, Type2>& max(const Type1& a,
                                                      const Type2& b) noexcept;

//! Return the small of two elements
template <typename Type1, typename Type2>
constexpr const std::common_type_t<Type1, Type2>& min(const Type1& a,
                                                      const Type2& b) noexcept;

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

//! Test whether the n is negative
template <Arithmetic Arith>
constexpr bool isNegative(const Arith x) noexcept;

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
