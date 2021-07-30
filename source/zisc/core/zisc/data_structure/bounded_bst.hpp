/*!
  \file bounded_bst.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BOUNDED_BST_HPP
#define ZISC_BOUNDED_BST_HPP

// Standard C++ library
#include <cstddef>
#include <tuple>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam BstClass No description.
  */
template <typename BstClass>
class BoundedBst : private NonCopyable<BoundedBst<BstClass>>
{
 public:
  // Type aliases for STL
  using size_type = std::size_t;


  //! Insert the given value into the bst
  template <ConvertibleTo<double> Type>
  std::tuple<bool, size_type> add(const Type& value);

  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Check if the given value is contained in the bst
  template <ConvertibleTo<double> Type>
  std::tuple<bool, size_type> contain(const Type& value) const noexcept;

  //! Find the minimum key in the bst
  double findMinKey() const noexcept;

  //! Remove the value from the bst
  template <ConvertibleTo<double> Type>
  std::tuple<bool, size_type> remove(const Type& value);

  //! Change the maximum possible number of elements. The bst will be cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements in the bst
  size_type size() const noexcept;

 protected:
  // Type aliases
  using BstT = std::remove_volatile_t<BstClass>;
  using ConstBstT = std::add_const_t<BstT>;
  using BstPtr = std::add_pointer_t<BstT>;
  using ConstBstPtr = std::add_pointer_t<ConstBstT>;
  using BstReference = std::add_lvalue_reference_t<BstT>;
  using ConstBstReference = std::add_lvalue_reference_t<ConstBstT>;


  //! Create a bst
  BoundedBst() noexcept;

  //! Move a data
  BoundedBst(const BoundedBst& other) noexcept;


  //! Move a data
  BoundedBst& operator=(const BoundedBst& other) noexcept;


  //! Return the reference to the bst class
  BstReference ref() noexcept;

  //! Return the reference to the bst class
  ConstBstReference ref() const noexcept;
};

} // namespace zisc

#include "bounded_bst-inl.hpp"

#endif // ZISC_BOUNDED_BST_HPP
