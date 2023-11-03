/*!
  \file algorithm.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
  static constexpr auto abs(const Arith x) noexcept -> Arith;

  //! Clamp the value \a x between \a lower and \a upper
  template <typename Type, typename LowerType, typename UpperType>
  static constexpr auto clamp(const Type value, 
                              const LowerType lower, 
                              const UpperType upper) noexcept
      -> std::common_type_t<Type, LowerType, UpperType>;

  //! Return a inverse number of the given value
  template <std::floating_point Float>
  static constexpr auto invert(const Float x) noexcept -> Float;

  //! Test whether a value is in a range
  template <bool kIsLeftClosed = true,
            bool kIsRightCloed = false,
            typename Type,
            typename LowerType,
            typename UpperType>
  static constexpr auto isInBounds(const Type& value, 
                                   const LowerType& lower, 
                                   const UpperType& upper) noexcept -> bool;

  //! Test whether a value is in a range: (\a lower, \a upper )
  template <typename Type, typename LowerType, typename UpperType>
  static constexpr auto isInOpenBounds(const Type& value, 
                                       const LowerType& lower, 
                                       const UpperType& upper) noexcept -> bool;

  //! Test whether a value is in a range: [\a lower, \a upper ]
  template <typename Type, typename LowerType, typename UpperType>
  static constexpr auto isInClosedBounds(const Type& value, 
                                         const LowerType& lower, 
                                         const UpperType& upper) noexcept -> bool;

  //! Test whether the given number is negative
  template <Arithmetic Arith>
  static constexpr auto isNegative(const Arith x) noexcept -> bool;

  //! Return the greater of two elements
  template <typename Type1, typename Type2>
  static constexpr auto max(const Type1& a,
                            const Type2& b) noexcept
      -> const std::common_type_t<Type1, Type2>&;

  //! Return the smaller of two elements
  template <typename Type1, typename Type2>
  static constexpr auto min(const Type1& a,
                            const Type2& b) noexcept
      -> const std::common_type_t<Type1, Type2>&;

  // Integer

  //! Return the sub-range of the given index 
  template <Integer Int1, Integer Int2, Integer Int3>
  static constexpr auto divideRange(const Int1 range,
                                    const Int2 num_of_division,
                                    const Int3 index) noexcept
      -> std::array<std::common_type_t<Int1, Int2, Int3>, 2>;

  //! Test whether the given value is odd
  template <Integer Int>
  static constexpr auto isOdd(const Int x) noexcept -> bool;
};

// STL style function aliases

//! Return the absolute value of the given value
template <Arithmetic Arith>
constexpr auto abs(const Arith x) noexcept -> Arith;

//! Clamp the value x to be between min and max
template <typename Type, typename LowerType, typename UpperType>
constexpr auto clamp(const Type value, 
                     const LowerType lower, 
                     const UpperType upper) noexcept
    -> std::common_type_t<Type, LowerType, UpperType>;

//! Return a inverse number
template <std::floating_point Float>
constexpr auto invert(const Float x) noexcept -> Float;

//! Return the large of two elements
template <typename Type1, typename Type2>
constexpr auto max(const Type1& a, const Type2& b) noexcept
    -> const std::common_type_t<Type1, Type2>&;

//! Return the small of two elements
template <typename Type1, typename Type2>
constexpr auto min(const Type1& a, const Type2& b) noexcept
    -> const std::common_type_t<Type1, Type2>&;

//! Test whether the given number is negative
template <Arithmetic Arith>
constexpr auto signbit(const Arith x) noexcept -> bool;

//

//! Test whether a value is in a range
template <bool kIsLeftClosed = true,
          bool kIsRightCloed = false,
          typename Type,
          typename LowerType,
          typename UpperType>
constexpr auto isInBounds(const Type& value, 
                          const LowerType& lower, 
                          const UpperType& upper) noexcept -> bool;

//! Test whether a value is in a range: (lower, upper)
template <typename Type, typename LowerType, typename UpperType>
constexpr auto isInOpenBounds(const Type& value, 
                              const LowerType& lower, 
                              const UpperType& upper) noexcept -> bool;

//! Test whether a value is in a range: [lower, upper]
template <typename Type, typename LowerType, typename UpperType>
constexpr auto isInClosedBounds(const Type& value, 
                                const LowerType& lower, 
                                const UpperType& upper) noexcept -> bool;

//! Return the sub-range of the given index 
template <Integer Int1, Integer Int2, Integer Int3>
constexpr auto divideRange(const Int1 range,
                           const Int2 num_of_division,
                           const Int3 index) noexcept
    -> std::array<std::common_type_t<Int1, Int2, Int3>, 2>;

//! Test whether the n is odd
template <Integer Int>
constexpr auto isOdd(const Int x) noexcept -> bool;

} // namespace zisc

/*!
  \example algorithm_example.cpp

  This is an example of how to use zisc::Algorithm.
  */

#include "algorithm-inl.hpp"

#endif // ZISC_ALGORITHM_HPP
