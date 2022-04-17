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
#include <concepts>
#include <cstddef>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "search_tree.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam Key No description.
  \tparam T No description.
  \tparam Compare No description.
  */
template <std::movable Key,
          MappedValue T = void,
          std::invocable<Key, Key> Compare = std::less<Key>>
class MutexBst : public SearchTree<MutexBst<Key, T, Compare>, Key, T, Compare>
{
 public:
  // Type aliases
  using BaseTreeT = SearchTree<MutexBst<Key, T, Compare>, Key, T, Compare>;
  using KeyT = typename BaseTreeT::KeyT;
  using ConstKeyT = typename BaseTreeT::ConstKeyT;
  using MappedT = typename BaseTreeT::MappedT;
  using ValueT = typename BaseTreeT::ValueT;
  using ConstValueT = typename BaseTreeT::ConstValueT;
  using CompareT = typename BaseTreeT::CompareT;
  using Reference = typename BaseTreeT::Reference;
  using RReference = typename BaseTreeT::RReference;
  using ConstReference = typename BaseTreeT::ConstReference;
  using Pointer = typename BaseTreeT::Pointer;
  using ConstPointer = typename BaseTreeT::ConstPointer;

  // Type aliases for STL
  using key_type = typename BaseTreeT::key_type;
  using mapped_type = typename BaseTreeT::mapped_type;
  using value_type = typename BaseTreeT::value_type;
  using size_type = typename BaseTreeT::size_type;
  using difference_type = typename BaseTreeT::difference_type;
  using key_compare = typename BaseTreeT::key_compare;
  using reference = typename BaseTreeT::reference;
  using const_reference = typename BaseTreeT::const_reference;
  using pointer = typename BaseTreeT::pointer;
  using const_pointer = typename BaseTreeT::const_pointer;


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
  [[nodiscard]] std::optional<size_type> add(ConstReference value);

  //! Insert the given value into the bst
  [[nodiscard]] std::optional<size_type> add(RReference value);

  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Check if the given value is contained in the bst
  [[nodiscard]] std::optional<size_type> contain(ConstKeyT& key) const noexcept;

  //! Return the default capacity
  static constexpr size_type defaultCapacity() noexcept;

  //! Find the minimum key in the bst
  [[nodiscard]] std::optional<Pointer> findMinKey() noexcept;

  //! Find the minimum key in the bst
  [[nodiscard]] std::optional<ConstPointer> findMinKey() const noexcept;

  //! Return the value by the given index
  Reference get(const size_type index) noexcept;

  //! Return the value by the given index
  ConstReference get(const size_type index) const noexcept;

  //! Check if the bst is bounded
  static constexpr bool isBounded() noexcept;

  //! Check if the bst is concurrent
  static constexpr bool isConcurrent() noexcept;

  //! Remove the value from the bst
  [[nodiscard]] std::optional<size_type> remove(ConstKeyT& key);

  //! Change the maximum possible number of elements. The bst will be cleared
  void setCapacity(const size_type cap) noexcept;

  //! Return the number of elements in the bst
  size_type size() const noexcept;

 private:
  using Memory = std::aligned_storage_t<sizeof(ValueT), std::alignment_of_v<ValueT>>;


  //! Compare the two keys
  static bool compare(ConstPointer lhs, ConstKeyT& rhs) noexcept;

  //! Compare the two keys of nodes
  static bool compareNode(ConstPointer lhs, ConstPointer rhs) noexcept;

  //! Check if the two given keys are same
  static bool equal(ConstKeyT& lhs, ConstKeyT& rhs) noexcept;

  //! Return the index of the given node
  size_type getIndex(ConstPointer node) const noexcept;

  //! Return the memory poiner by the given index
  Pointer getMemory(const size_type index) noexcept;

  //! Return the memory poiner by the given index
  ConstPointer getMemory(const size_type index) const noexcept;

  //! Return the invalid id
  static constexpr size_type invalidId() noexcept;


  mutable std::shared_mutex mutex_;
  pmr::vector<size_type> index_stack_;
  pmr::vector<Memory> node_pool_;
  pmr::vector<Pointer> node_list_;
};

} // namespace zisc

#include "mutex_bst-inl.hpp"

#endif // ZISC_MUTEX_BST_HPP
