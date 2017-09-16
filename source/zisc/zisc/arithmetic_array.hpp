/*!
  \file arithmetic_array.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ARITHMETIC_ARRAY_HPP
#define ZISC_ARITHMETIC_ARRAY_HPP

// Standard C++ library
#include <array>
#include <cstddef>
#include <type_traits>
// Zisc
#include "array.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN>
class ArithmeticArray
{
  static_assert(std::is_arithmetic<Arithmetic>::value,
                "The Arithmetic isn't arithmetic type.");
  using BaseArray = Array<Arithmetic, kN>;

 public:
  // For STL
  using value_type = typename BaseArray::value_type;
  using size_type = typename BaseArray::size_type;
  using difference_type = typename BaseArray::difference_type;
  using reference = typename BaseArray::reference;
  using const_reference = typename BaseArray::const_reference;
  using pointer = typename BaseArray::pointer;
  using const_pointer = typename BaseArray::const_pointer;
  using iterator = typename BaseArray::iterator;
  using const_iterator = typename BaseArray::const_iterator;


  //! Initialize all elements with 0
  constexpr ArithmeticArray() noexcept;

  //! Initialize with values
  template <typename ...Types>
  constexpr ArithmeticArray(const Types ...values) noexcept;

  //! Initialize with a std::array
  ArithmeticArray(const std::array<Arithmetic, kN>& other) noexcept;

  //! Initialize with a array
  constexpr ArithmeticArray(const ArithmeticArray& other) noexcept;


  // For STL
  //! Return an iterator to the first element of the container
  constexpr iterator begin() noexcept;

  //! Return an iterator to the first element of the container
  constexpr const_iterator begin() const noexcept;

  //! Return an iterator to the first element of the container
  constexpr const_iterator cbegin() const noexcept;

  //! Return an iterator following the last element of the container
  constexpr iterator end() noexcept;

  //! Return an iterator following the last element of the container
  constexpr const_iterator end() const noexcept;

  //! Return an iterator following the last element of the container
  constexpr const_iterator cend() const noexcept;


  //! Apply addition operation to each element in the array
  constexpr ArithmeticArray operator+(const ArithmeticArray& other) const noexcept;

  //! Apply subtraction operation to each element in the array
  constexpr ArithmeticArray operator-(const ArithmeticArray& other) const noexcept;

  //! Multiply each element by a scalar
  constexpr ArithmeticArray operator*(const Arithmetic scalar) const noexcept;

  //! Apply multiplication operation to each element in the array
  constexpr ArithmeticArray operator*(const ArithmeticArray& other) const noexcept;

  //! Divide each element by a scalar
  constexpr ArithmeticArray operator/(const Arithmetic scalar) const noexcept;

  //! Apply division operation to each element in the array
  constexpr ArithmeticArray operator/(const ArithmeticArray& other) const noexcept;

  //!
  constexpr ArithmeticArray& operator=(const ArithmeticArray& other) noexcept;

  //!
  constexpr ArithmeticArray& operator+=(const ArithmeticArray& other) noexcept;

  //!
  constexpr ArithmeticArray& operator-=(const ArithmeticArray& other) noexcept;

  //!
  constexpr ArithmeticArray& operator*=(const Arithmetic scalar) noexcept;

  //!
  constexpr ArithmeticArray& operator*=(const ArithmeticArray& other) noexcept;

  //!
  constexpr ArithmeticArray& operator/=(const Arithmetic scalar) noexcept;

  //!
  constexpr ArithmeticArray& operator/=(const ArithmeticArray& other) noexcept;

  //! Return the reference by index
  constexpr Arithmetic& operator[](const uint index) noexcept;

  //! Return the reference by index
  constexpr const Arithmetic& operator[](const uint index) const noexcept;


  //! Clamp all elements
  constexpr void clampAll(const Arithmetic min_value,
                          const Arithmetic max_value) noexcept;

  //! Divide a scalar by a array
  constexpr ArithmeticArray divideScalar(const Arithmetic scalar) const noexcept;

  //! Fill the container with specified value
  constexpr void fill(const Arithmetic& value) noexcept;

  //! Return the reference by index
  constexpr Arithmetic& get(const uint index) noexcept;

  //! Return the reference by index
  constexpr const Arithmetic& get(const uint index) const noexcept;

  //! Check whether the array has inf
  bool hasInf() const noexcept;

  //! Check whether the array has nan
  bool hasNan() const noexcept;

  //! Check whether the array has the specified value
  constexpr bool hasValue(const Arithmetic value) const noexcept;

  //! Check whether all elements are between [ \p lower , \p upper )
  constexpr bool isAllInBounds(const Arithmetic lower,
                               const Arithmetic upper) const noexcept;

  //! Check whether all elements are between [ \p lower , \p upper ]
  constexpr bool isAllInClosedBounds(const Arithmetic lower,
                                     const Arithmetic upper) const noexcept;

  //! Check whether all elements are zero
  constexpr bool isAllZero() const noexcept;

  //! Return the largest element
  constexpr Arithmetic max() const noexcept;

  //! Return the smallest element
  constexpr Arithmetic min() const noexcept;

  //! Compare two arrays and store maximum values
  constexpr ArithmeticArray maxElements(const ArithmeticArray& other) const noexcept;

  //! Compare two arrays and store minimum values
  constexpr ArithmeticArray minElements(const ArithmeticArray& other) const noexcept;

  //! Return the number of elements
  static constexpr size_type size() noexcept;

  //! Set value
  void set(const uint index, const Arithmetic& value) noexcept;

  //! Set values
  template <typename ...Types>
  void setElements(Types&& ...values) noexcept;

  //! Return the sum of elements
  Arithmetic sum() const noexcept;

 private:
  //! Set value
  template <uint index, typename ...Types>
  void setElements(const Arithmetic& value, Types&& ...values) noexcept;

  //! Set value
  template <uint index>
  void setElements(const Arithmetic& value) noexcept;

  //! Return the sum of integer elements
  template <typename Type>
  static Type sumArray(const BaseArray& array,
                       EnableIfInteger<Type> = kEnabler) noexcept;

  //! Return the sum of float elements
  template <typename Type>
  static Type sumArray(const BaseArray& array,
                       EnableIfFloat<Type> = kEnabler) noexcept;


  BaseArray array_;
};

//! Check whether two arrays are same
template <typename Arithmetic, uint kN>
constexpr bool operator==(
    const ArithmeticArray<Arithmetic, kN>& a, 
    const ArithmeticArray<Arithmetic, kN>& b) noexcept;

//! Check whether two arrays are not same
template <typename Arithmetic, uint kN>
constexpr bool operator!=(
    const ArithmeticArray<Arithmetic, kN>& a, 
    const ArithmeticArray<Arithmetic, kN>& b) noexcept;

//! Multiply each element by a scalar
template <typename Arithmetic, uint kN>
constexpr ArithmeticArray<Arithmetic, kN> operator*(
    const Arithmetic scalar,
    const ArithmeticArray<Arithmetic, kN>& array) noexcept;

//! Divide a scalar by a array
template <typename Arithmetic, uint kN>
constexpr ArithmeticArray<Arithmetic, kN> operator/(
    const Arithmetic scalar,
    const ArithmeticArray<Arithmetic, kN>& array) noexcept;

//! Calculate the cross product
template <typename Arithmetic, uint kN>
constexpr ArithmeticArray<Arithmetic, kN> cross(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b) noexcept;

//! Calculate the inner product
template <typename Arithmetic, uint kN>
constexpr Arithmetic dot(
    const ArithmeticArray<Arithmetic, kN>& a, 
    const ArithmeticArray<Arithmetic, kN>& b) noexcept;

//! Compare two arrays and return maximum values
template <typename Arithmetic, uint kN>
constexpr ArithmeticArray<Arithmetic, kN> maxElements(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b) noexcept;

//! Compare two arrays and return minimum values
template <typename Arithmetic, uint kN>
constexpr ArithmeticArray<Arithmetic, kN> minElements(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b) noexcept;

//! Compare two arrays and store minimu values to a and maximum values to b
template <typename Arithmetic, uint kN>
constexpr void minMaxElements(
    ArithmeticArray<Arithmetic, kN>& a,
    ArithmeticArray<Arithmetic, kN>& b) noexcept;

//! Swap two array elements by index
template <typename Arithmetic, uint kN>
constexpr void swapElement(
    const ArithmeticArray<Arithmetic, kN>& a,
    const ArithmeticArray<Arithmetic, kN>& b,
    const uint index) noexcept;

} // namespace zisc

#include "arithmetic_array-inl.hpp"

#endif // ZISC_ARITHMETIC_ARRAY_HPP
