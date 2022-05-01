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
#include "map.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"
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
class MutexBst : public Map<MutexBst<Key, T, Compare>, Key, T, Compare>
{
 public:
  // Type aliases
  using BaseMapT = Map<MutexBst<Key, T, Compare>, Key, T, Compare>;
  using KeyT = typename BaseMapT::KeyT;
  using ConstKeyT = typename BaseMapT::ConstKeyT;
  using MappedT = typename BaseMapT::MappedT;
  using ValueT = typename BaseMapT::ValueT;
  using ConstValueT = typename BaseMapT::ConstValueT;
  using CompareT = typename BaseMapT::CompareT;
  using Reference = typename BaseMapT::Reference;
  using RReference = typename BaseMapT::RReference;
  using ConstReference = typename BaseMapT::ConstReference;
  using Pointer = typename BaseMapT::Pointer;
  using ConstPointer = typename BaseMapT::ConstPointer;

  // Type aliases for STL
  using key_type = typename BaseMapT::key_type;
  using mapped_type = typename BaseMapT::mapped_type;
  using value_type = typename BaseMapT::value_type;
  using size_type = typename BaseMapT::size_type;
  using difference_type = typename BaseMapT::difference_type;
  using key_compare = typename BaseMapT::key_compare;
  using reference = typename BaseMapT::reference;
  using const_reference = typename BaseMapT::const_reference;
  using pointer = typename BaseMapT::pointer;
  using const_pointer = typename BaseMapT::const_pointer;


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
  template <typename ...Args>
  [[nodiscard]] std::optional<size_type> add(Args&&... args)
      requires std::is_nothrow_constructible_v<ValueT, Args...>;

  //! Return the maximum possible number of elements
  size_type capacity() const noexcept;

  //! Return the maximum possible capacity
  static constexpr size_type capacityMax() noexcept;

  //! Clear the contents
  void clear() noexcept;

  //! Check if the given value is contained in the bst
  [[nodiscard]] std::optional<size_type> contain(ConstKeyT& key) const noexcept;

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

  //! Return a pointer to the underlying memory resource
  pmr::memory_resource* resource() const noexcept;

  //! Change the maximum possible number of elements. The bst will be cleared
  void setCapacity(size_type cap) noexcept;

  //! Return the number of elements in the bst
  size_type size() const noexcept;

 private:
  using StorageT = DataStorage<ValueT>;
  using ConstStorageT = std::add_const_t<StorageT>;
  using StorageRef = std::add_lvalue_reference_t<StorageT>;
  using ConstStorageRef = std::add_lvalue_reference_t<ConstStorageT>;
  using StoragePtr = std::add_pointer_t<StorageT>;
  using ConstStoragePtr = std::add_pointer_t<ConstStorageT>;


  //! Compare the two keys
  static bool compare(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept;

  //! Compare the two keys of nodes
  static bool compareNode(ConstStoragePtr lhs, ConstStoragePtr rhs) noexcept;

  //! Check if the two given keys are same
  static bool equal(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept;

  //! Return the index of the given node
  size_type getIndex(ConstStoragePtr node) const noexcept;

  //! Return the storage by the given index
  StorageRef getStorage(const size_type index) noexcept;

  //! Return the storage by the given index
  ConstStorageRef getStorage(const size_type index) const noexcept;

  //! Return the invalid id
  static constexpr size_type invalidId() noexcept;

  //! Issue a storage index from the index stack
  size_type issueStorageIndex() noexcept;


  mutable std::shared_mutex mutex_;
  pmr::vector<size_type> index_stack_;
  pmr::vector<StorageT> node_pool_;
  pmr::vector<StoragePtr> node_list_;
};

} // namespace zisc

#include "mutex_bst-inl.hpp"

#endif // ZISC_MUTEX_BST_HPP
