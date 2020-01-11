/*!
  \file arith_array.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  No copyright
  */

#ifndef ZISC_ARITH_ARRAY_HPP
#define ZISC_ARITH_ARRAY_HPP

// Standard C++ library
#include <array>
#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam Arith No description.
  \tparam kN No description.
  */
template <typename Arith, uint kN>
class ArithArray
{
  static_assert(std::is_arithmetic_v<Arith>, "Arith isn't arithmetic type.");
  using ArrayType = std::array<Arith, kN>; //!< Array data type

 public:
  // Type aliases for STL
  using value_type = typename ArrayType::value_type;
  using size_type = uint;
  using difference_type = typename ArrayType::difference_type;
  using reference = typename ArrayType::reference;
  using const_reference = typename ArrayType::const_reference;
  using pointer = typename ArrayType::pointer;
  using const_pointer = typename ArrayType::const_pointer;
  using iterator = typename ArrayType::iterator;
  using const_iterator = typename ArrayType::const_iterator;
  using reverse_iterator = typename ArrayType::reverse_iterator;
  using const_reverse_iterator = typename ArrayType::const_reverse_iterator;


  //! Initialize the elements of the array with 0
  constexpr ArithArray() noexcept;

  //! Initialize the elements
  constexpr ArithArray(const std::initializer_list<Arith> value_list) noexcept;

  //! Copy array
  constexpr ArithArray(const ArrayType& other) noexcept;

  //! Move array
  constexpr ArithArray(ArrayType&& other) noexcept;


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


  //!
  constexpr ArithArray& operator+=(const ArithArray& other) noexcept;

  //!
  constexpr ArithArray& operator-=(const ArithArray& other) noexcept;

  //!
  constexpr ArithArray& operator*=(const Arith scalar) noexcept;

  //!
  constexpr ArithArray& operator*=(const ArithArray& other) noexcept;

  //!
  constexpr ArithArray& operator/=(const Arith scalar) noexcept;

  //!
  constexpr ArithArray& operator/=(const ArithArray& other) noexcept;

  //! Return the reference by index
  constexpr reference operator[](const uint index) noexcept;

  //! Return the reference by index
  constexpr const_reference operator[](const uint index) const noexcept;


  //! Clamp all elements between \a lower and \a upper
  constexpr void clampAll(const Arith lower, const Arith upper) noexcept;

  //! Fill the container with specified value
  constexpr void fill(const Arith& value) noexcept;

  //! Return the reference by index
  constexpr reference get(const uint index) noexcept;

  //! Return the reference by index
  constexpr const_reference get(const uint index) const noexcept;

  //! Check whether the array has inf value
  constexpr bool hasInf() const noexcept;

  //! Check whether the array has nan value
  constexpr bool hasNan() const noexcept;

  //! Check whether the array has subnormal value
  constexpr bool hasSubnormal() const noexcept;

  //! Check whether the array has the specified value
  constexpr bool hasValue(const Arith value) const noexcept;

  //! Check whether all elements are between [ \p lower , \p upper )
  constexpr bool isAllInBounds(const Arith lower,
                               const Arith upper) const noexcept;

  //! Check whether all elements are between [ \p lower , \p upper ]
  constexpr bool isAllInClosedBounds(const Arith lower,
                                     const Arith upper) const noexcept;

  //! Check whether all elements are zero
  constexpr bool isAllZero() const noexcept;

  //! Return the largest element
  constexpr Arith max() const noexcept;

  //! Return the smallest element
  constexpr Arith min() const noexcept;

  //! Set value
  constexpr void set(const uint index, const Arith value) noexcept;

  //! Set values
  constexpr void set(std::initializer_list<Arith> value_list) noexcept;

  //! Return the number of elements
  static constexpr size_type size() noexcept;

  //! Return the sum of elements
  constexpr Arith sum() const noexcept;

 private:
  //! Make a array
  template <std::size_t ...indices>
  static constexpr ArrayType makeArray(
      const std::initializer_list<Arith> value_list,
      const std::index_sequence<indices...>) noexcept;


  ArrayType data_;
};

//! Perform unary negation operation
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator-(const ArithArray<Arith, kN>& array) noexcept;

//! Perform addition operation on two arrays
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator+(const ArithArray<Arith, kN>& lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept;

//! Perform subtraction operation on two arrays
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator-(const ArithArray<Arith, kN>& lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept;

//! Perform multiplication operation on two arrays
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator*(const ArithArray<Arith, kN>& lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept;

//! Perform multiplication operation on value and array
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator*(const Arith lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept;

//! Perform multiplication operation on value and array
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator*(const ArithArray<Arith, kN>& lhs,
                                          const Arith rhs) noexcept;

//! Perform division operation on two arrays
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator/(const ArithArray<Arith, kN>& lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept;

//! Perform division operation on two arrays
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator/(const Arith lhs,
                                          const ArithArray<Arith, kN>& rhs) noexcept;

//! Perform division operation on two arrays
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> operator/(const ArithArray<Arith, kN>& lhs,
                                          const Arith rhs) noexcept;

//! Check whether two arrays are same
template <typename Arith, uint kN>
constexpr bool operator==(const ArithArray<Arith, kN>& lhs, 
                          const ArithArray<Arith, kN>& rhs) noexcept;

//! Check whether two arrays are not same
template <typename Arith, uint kN>
constexpr bool operator!=(const ArithArray<Arith, kN>& lhs, 
                          const ArithArray<Arith, kN>& rhs) noexcept;

//! Calculate the inner product
template <typename Arith, uint kN>
constexpr Arith dot(const ArithArray<Arith, kN>& lhs, 
                    const ArithArray<Arith, kN>& rhs) noexcept;

//! Calculate the cross product
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> cross(const ArithArray<Arith, kN>& lhs,
                                      const ArithArray<Arith, kN>& rhs) noexcept;

//! Compare two arrays and return maximum values
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> maxElements(
    const ArithArray<Arith, kN>& lhs,
    const ArithArray<Arith, kN>& rhs) noexcept;

//! Compare two arrays and return minimum values
template <typename Arith, uint kN>
constexpr ArithArray<Arith, kN> minElements(
    const ArithArray<Arith, kN>& lhs,
    const ArithArray<Arith, kN>& rhs) noexcept;

//! Compare two arrays and store minimu values to lhs and maximum values to rhs
template <typename Arith, uint kN>
constexpr void minMaxElements(ArithArray<Arith, kN>& lhs,
                              ArithArray<Arith, kN>& rhs) noexcept;

//! Swap two array elements by index
template <typename Arith, uint kN>
constexpr void swapElement(const ArithArray<Arith, kN>& lhs,
                           const ArithArray<Arith, kN>& rhs,
                           const uint index) noexcept;

} // namespace zisc

#include "arith_array-inl.hpp"

#endif // ZISC_ARITH_ARRAY_HPP
