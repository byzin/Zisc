/*!
  \file array.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ARRAY_HPP
#define ZISC_ARRAY_HPP

// Standard C++ library
#include <array>
#include <cstddef>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Type, uint kN>
class Array
{
  static_assert(0 < kN, "kN is 0.");
 public:
  // For STL
  using value_type = Type;
  using size_type = uint;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = pointer;
  using const_iterator = const_pointer;


  //! Initialize all elements with 0
  constexpr Array() noexcept;

  //! Initialize with values
  template <typename ...Types>
  constexpr Array(Types&& ...values) noexcept;

  //! Initialize with a array
  Array(const std::array<Type, kN>& other) noexcept;

  //! Move data from the other array
  Array(std::array<Type, kN>&& other) noexcept;

  //! Initialize with a array
  constexpr Array(const Array& other) noexcept;

  //! Move data from the other array
  constexpr Array(Array&& other) noexcept;


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


  //! Copy a array
  constexpr Array& operator=(const Array& other) noexcept;

  //! Move a array
  constexpr Array& operator=(Array&& other) noexcept;

  //! Return the reference by index
  constexpr Type& operator[](const uint index) noexcept;

  //! Return the reference by index
  constexpr const Type& operator[](const uint index) const noexcept;


  //! Fill the container with specified value
  constexpr void fill(const Type& value) noexcept;

  //! Return the reference by index.
  constexpr Type& get(const uint index) noexcept;

  //! Return the reference by index.
  constexpr const Type& get(const uint index) const noexcept;

  //! Return the number of elements
  static constexpr size_type size() noexcept;

  //! Set value
  void set(const uint index, const Type& value) noexcept;

  //! Set value
  void set(const uint index, Type&& value) noexcept;

  //! Set values
  template <typename ...Types>
  void setElements(Types&& ...values) noexcept;

 private:
  //! Set value
  template <uint index, typename ...Types>
  void setElements(const Type& value, Types&& ...values) noexcept;

  //! Set value
  template <uint index, typename ...Types>
  void setElements(Type&& value, Types&& ...values) noexcept;

  //! Set value
  template <uint index>
  void setElements(const Type& value) noexcept;

  //! Set value
  template <uint index>
  void setElements(Type&& value) noexcept;

  struct
  {
    Type elements_[kN];
  } array_;
};

//! Swap two array elements by index
template <typename Type, uint kN>
constexpr void swapElement(const Array<Type, kN>& a,
                           const Array<Type, kN>& b,
                           const uint index) noexcept;

} // namespace zisc

#include "array-inl.hpp"

#endif // ZISC_ARRAY_HPP
