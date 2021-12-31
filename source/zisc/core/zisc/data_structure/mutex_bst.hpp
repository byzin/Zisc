/*!
  \file mutex_bst.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_BST_HPP
#define ZISC_MUTEX_BST_HPP

// Standard C++ library
#include <cstddef>
#include <shared_mutex>
#include <tuple>
#include <vector>
// Zisc
#include "mutex_bst_node.hpp"
#include "search_tree.hpp"
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class MutexBst : public SearchTree<MutexBst>
{
 public:
  // Type aliases
  using BaseBstType = SearchTree<MutexBst>;

  // Type aliases for STL
  using size_type = typename BaseBstType::size_type;


  //! Create a bst
  MutexBst(pmr::memory_resource* mem_resource) noexcept;

  //! Create a bst
  MutexBst(const size_type cap, pmr::memory_resource* memory_resource) noexcept;

  //! Move a data
  MutexBst(MutexBst&& other) noexcept;

  //! Destroy the bst
  ~MutexBst() noexcept;


  //! Move a data
  MutexBst& operator=(MutexBst&& other) noexcept;


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

  //! Return the default capacity
  static constexpr size_type defaultCapacity() noexcept;

  //! Find the minimum key in the bst
  double findMinKey() const noexcept;

  //! Check if the bst is concurrent
  static constexpr bool isConcurrent() noexcept;

  //! Remove the value from the bst
  template <ConvertibleTo<double> Type>
  std::tuple<bool, size_type> remove(const Type& value);

  //! Change the maximum possible number of elements. The bst will be cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements in the bst
  size_type size() const noexcept;

 private:
  // Type aliases
  using Node = MutexBstNode;


  //! Compare the given node and the key
  static bool compare(const Node& lhs, const double rhs) noexcept;

  //! Return the invalid id
  static constexpr size_type invalidId() noexcept;


  mutable std::shared_mutex mutex_;
  pmr::vector<Node> node_list_;
  pmr::vector<size_type> index_stack_;
};

} // namespace zisc

#include "mutex_bst-inl.hpp"

#endif // ZISC_MUTEX_BST_HPP
