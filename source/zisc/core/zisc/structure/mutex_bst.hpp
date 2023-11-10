/*!
  \file mutex_bst.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_BST_HPP
#define ZISC_MUTEX_BST_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory_resource>
#include <optional>
#include <shared_mutex>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "map.hpp"
#include "mutex_bst_iterator.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"

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
  using Iterator = MutexBstIterator<Key, T>;
  using ConstIterator = MutexBstIterator<ConstKeyT, std::add_const_t<T>>;

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
  using iterator = Iterator;
  using const_iterator = ConstIterator;


  //! Create a bst
  explicit MutexBst(std::pmr::memory_resource* mem_resource) noexcept;

  //! Create a bst
  MutexBst(const size_type cap, std::pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  MutexBst(MutexBst&& other) noexcept;

  //! Destroy the bst
  ~MutexBst() noexcept;


  //! Move a data
  auto operator=(MutexBst&& other) noexcept -> MutexBst&;


  //! Return an iterator to the beginning
  auto begin() noexcept -> Iterator;

  //! Return an iterator to the beginning
  auto cbegin() const noexcept -> ConstIterator;

  //! Return an iterator to the end
  auto end() noexcept -> Iterator;

  //! Return an iterator to the end
  auto cend() const noexcept -> ConstIterator;


  //! Insert the given value into the bst
  template <typename ...Args>
  [[nodiscard]]
  auto add(Args&&... args) -> std::optional<size_type>;

  //! Return the maximum possible number of elements
  auto capacity() const noexcept -> size_type;

  //! Return the maximum possible capacity
  static constexpr auto capacityMax() noexcept -> size_type;

  //! Clear the contents
  void clear() noexcept;

  //! Check if the given value is contained in the bst
  [[nodiscard]]
  auto contain(ConstKeyT& key) const noexcept -> std::optional<size_type>;

  //! Find the minimum key in the bst
  [[nodiscard]]
  auto findMinKey() noexcept -> std::optional<Pointer>;

  //! Find the minimum key in the bst
  [[nodiscard]]
  auto findMinKey() const noexcept -> std::optional<ConstPointer>;

  //! Return the value by the given index
  auto get(const size_type index) noexcept -> Reference;

  //! Return the value by the given index
  auto get(const size_type index) const noexcept -> ConstReference;

  //! Check if the bst is bounded
  static constexpr auto isBounded() noexcept -> bool;

  //! Check if the bst is concurrent
  static constexpr auto isConcurrent() noexcept -> bool;

  //! Remove the value from the bst
  [[nodiscard]]
  auto remove(ConstKeyT& key) -> std::optional<size_type>;

  //! Return a pointer to the underlying memory resource
  auto resource() const noexcept -> std::pmr::memory_resource*;

  //! Change the maximum possible number of elements. The bst will be cleared
  void setCapacity(size_type cap) noexcept;

  //! Return the number of elements in the bst
  auto size() const noexcept -> size_type;

 private:
  using StorageT = DataStorage<ValueT>;
  using ConstStorageT = std::add_const_t<StorageT>;
  using StorageRef = std::add_lvalue_reference_t<StorageT>;
  using ConstStorageRef = std::add_lvalue_reference_t<ConstStorageT>;
  using StoragePtr = std::add_pointer_t<StorageT>;
  using ConstStoragePtr = std::add_pointer_t<ConstStorageT>;


  //! Compare the two keys
  static auto compare(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept -> bool;

  //! Compare the two keys of nodes
  static auto compareNode(ConstStoragePtr lhs, ConstStoragePtr rhs) noexcept -> bool;

  //! Check if the two given keys are same
  static auto equal(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept -> bool;

  //! Return the index of the given node
  auto getIndex(ConstStoragePtr node) const noexcept -> size_type;

  //! Return the storage by the given index
  auto getStorage(const size_type index) noexcept -> StorageRef;

  //! Return the storage by the given index
  auto getStorage(const size_type index) const noexcept -> ConstStorageRef;

  //! Return the invalid id
  static constexpr auto invalidId() noexcept -> size_type;

  //! Issue a storage index from the index stack
  auto issueStorageIndex() noexcept -> size_type;


  mutable std::shared_mutex mutex_;
  std::pmr::vector<size_type> index_stack_;
  std::pmr::vector<StorageT> node_pool_;
  std::pmr::vector<StoragePtr> node_list_;
};

} // namespace zisc

#include "mutex_bst-inl.hpp"

#endif // ZISC_MUTEX_BST_HPP
