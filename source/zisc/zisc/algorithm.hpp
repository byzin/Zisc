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
#include "memory/simple_memory_resource.hpp"
#include "memory/std_memory_resource.hpp"
#include "type_traits.hpp"
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
  template <typename Arithmetic>
  static constexpr Arithmetic abs(const Arithmetic x) noexcept;

  //! Clamp the value \a x between \a lower and \a upper
  template <typename Type, typename LowerType, typename UpperType>
  static constexpr std::common_type_t<Type, LowerType, UpperType> clamp(
      const Type value, 
      const LowerType lower, 
      const UpperType upper) noexcept;

  //! Compute the number of leading zero bits in the given value 
  template <typename Integer>
  static Integer clz(const Integer x) noexcept;

  //! Return a inverse number of the given value
  template <typename Float>
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
  template <typename Arithmetic>
  static constexpr bool isNegative(const Arithmetic x) noexcept;

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

  //! Return the exponent corresponds to the given value in the base-2
  template <typename Integer>
  static constexpr Integer getExponent(Integer x) noexcept;

  //! Test whether the given value is odd
  template <typename Integer>
  static constexpr bool isOdd(const Integer x) noexcept;

  //! Check if the given value is power of 2
  template <typename Integer>
  static constexpr bool isPowerOf2(const Integer x) noexcept;

  //! Compute the number of non-zero bits in the given value 
  template <typename Integer>
  static Integer popcount(const Integer x) noexcept;

  //! Compute the smallest power of 2 that is greater or equal to the given value
  template <typename Integer>
  static constexpr Integer roundUpToPowOf2(const Integer x) noexcept;

  // Binary search operations

  //! Return the iterator pointing that the element is greatest <= value
  template <typename RandomAccessIterator, typename Type>
  static RandomAccessIterator searchBinaryTree(
      const RandomAccessIterator begin,
      const RandomAccessIterator end,
      const Type& value) noexcept;

  //! Transform a sorted unique array to a binary tree
  template <typename RandomAccessIterator>
  static void toBinaryTree(RandomAccessIterator begin,
                           RandomAccessIterator end) noexcept;

  //! STL implementation
  class Stl
  {
   public:
    //! Return the iterator pointing that the element is greatest <= value
    template <typename RandomAccessIterator, typename Type>
    static RandomAccessIterator searchBinaryTree(
        const RandomAccessIterator begin,
        const RandomAccessIterator end,
        const Type& value) noexcept;

    //! Transform a sorted unique array to a binary tree
    template <typename RandomAccessIterator>
    static void toBinaryTree(RandomAccessIterator begin,
                             RandomAccessIterator end) noexcept;
  };

  //! Zisc implementation
  class Zisc
  {
   public:
    //! Compute the number of leading zero bits in the given value 
    template <typename Integer>
    static constexpr Integer clz(const Integer x) noexcept;

    //! Return the number of non-zero bits in the given x
    template <typename Integer>
    static constexpr Integer popcount(const Integer x) noexcept;


    //! Return the iterator pointing that the element is greatest <= value
    template <typename RandomAccessIterator, typename Type>
    static RandomAccessIterator searchBinaryTree(
        const RandomAccessIterator begin,
        const RandomAccessIterator end,
        const Type& value) noexcept;

    //! Transform a sorted unique array to a binary tree
    template <typename RandomAccessIterator>
    static void toBinaryTree(RandomAccessIterator begin,
                             RandomAccessIterator end) noexcept;

   private:
    //! Return a mask of popcount
    template <typename Integer>
    static constexpr Integer makePopcountMask(const std::size_t stage) noexcept;

    //! Return the number of non-zero bits in the given x
    template <typename Integer, std::size_t kStage>
    static constexpr Integer popcountImpl(const Integer x) noexcept;


    //! Transform a sorted unique array to a binary tree
    template <typename RandomAccessIterator, typename OutputIterator>
    static void toBinaryTreeImpl(RandomAccessIterator begin,
                                 RandomAccessIterator end,
                                 const std::size_t index,
                                 OutputIterator first) noexcept;
  };

 private:
  //!
  template <typename SrcInteger, typename DstInteger>
  static constexpr DstInteger makeClzMask() noexcept;

  //! Compute the number of leading zero bits in the given value 
  template <Config::ImplType kImpl, typename Integer>
  static Integer clzImpl(const Integer x) noexcept;

  //! Compute the number of leading zero bits in the given value 
  static short clzMsvcImpl(const short x) noexcept;

  //! Compute the number of leading zero bits in the given value 
  static unsigned short clzMsvcImpl(const unsigned short x) noexcept;

  //! Compute the number of leading zero bits in the given value 
  static int clzMsvcImpl(const int x) noexcept;

  //! Compute the number of leading zero bits in the given value 
  static unsigned int clzMsvcImpl(const unsigned int x) noexcept;

  //! Compute the number of leading zero bits in the given value 
  static long long clzMsvcImpl(const long long x) noexcept;

  //! Compute the number of leading zero bits in the given value 
  static unsigned long long clzMsvcImpl(const unsigned long long x) noexcept;

  //! Return the number of non-zero bits in the given x
  template <Config::ImplType kImpl, typename Integer>
  static Integer popcountImpl(const Integer x) noexcept;

  //! Return the number of non-zero bits in the given x
  static short popcountMsvcImpl(const short x) noexcept;

  //! Return the number of non-zero bits in the given x
  static unsigned short popcountMsvcImpl(const unsigned short x) noexcept;

  //! Return the number of non-zero bits in the given x
  static int popcountMsvcImpl(const int x) noexcept;

  //! Return the number of non-zero bits in the given x
  static unsigned int popcountMsvcImpl(const unsigned int x) noexcept;

  //! Return the number of non-zero bits in the given x
  static long long popcountMsvcImpl(const long long x) noexcept;

  //! Return the number of non-zero bits in the given x
  static unsigned long long popcountMsvcImpl(const unsigned long long x) noexcept;
};

// STL style function aliases

//! Return the absolute value of the given value
template <typename Arithmetic>
constexpr Arithmetic abs(const Arithmetic x) noexcept;

//! Clamp the value x to be between min and max
template <typename Type, typename LowerType, typename UpperType>
constexpr std::common_type_t<Type, LowerType, UpperType> clamp(
    const Type value, 
    const LowerType lower, 
    const UpperType upper) noexcept;

//! Return a inverse number
template <typename Float>
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
template <typename Arithmetic>
constexpr bool isNegative(const Arithmetic x) noexcept;

//! Test whether the n is odd
template <typename Integer>
constexpr bool isOdd(const Integer x) noexcept;

} // namespace zisc

/*!
  \example algorithm_example.cpp

  This is an example of how to use zisc::Algorithm.
  */

#include "algorithm-inl.hpp"

#endif // ZISC_ALGORITHM_HPP
