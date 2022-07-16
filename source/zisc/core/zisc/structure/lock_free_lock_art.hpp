/*!
  \file lock_free_lock_art.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_LOCK_ART_HPP
#define ZISC_LOCK_FREE_LOCK_ART_HPP

// Standard C++ library
#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <optional>
#include <span>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "map.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/internal/flock/epoch.hpp"
#include "zisc/internal/flock/lock.hpp"
#include "zisc/internal/flock/memory_pool.hpp"
#include "zisc/internal/flock/mutable.hpp"
#include "zisc/internal/flock/write_once.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief Lock Free Lock Adaptive Radix Tree

  For more details, please see the following paper:
  <a href="https://arxiv.org/abs/2201.00813">Lock-Free Locks Revisited</a>. <br>

  \tparam Key No description.
  \tparam T No description.
  \tparam Compare No description.
  */
template <std::movable Key,
          MappedValue T = void,
          std::invocable<Key, Key> Compare = std::less<Key>>
class LockFreeLockArt : public Map<LockFreeLockArt<Key, T, Compare>, Key, T, Compare>
{
 public:
  // Type aliases
  using BaseMapT = Map<LockFreeLockArt<Key, T, Compare>, Key, T, Compare>;
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
  LockFreeLockArt(const std::span<std::thread::id> thread_id_list,
                  pmr::memory_resource* mem_resource) noexcept;

  //! Create a bst
  LockFreeLockArt(const std::span<std::thread::id> thread_id_list,
                  const size_type cap,
                  pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  LockFreeLockArt(LockFreeLockArt&& other) noexcept;

  //! Destroy the bst
  ~LockFreeLockArt() noexcept;


  //! Move a data
  LockFreeLockArt& operator=(LockFreeLockArt&& other) noexcept;


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
  /*!
    \brief No brief description

    No detailed description.
    */
  enum class NodeType : uint8b
  {
    kFull,
    kIndirect,
    kSparse,
    kLeaf
  };


  // Type aliases
  struct Node;
  using ConstNodeT = std::add_const_t<Node>;
  using NodePtrT = std::add_pointer_t<Node>;
  using ConstNodePtrT = std::add_pointer_t<ConstNodeT>;
  using NodePtrMutableT = flock::Mutable<NodePtrT>;

  /*!
    \brief No brief description

    No detailed description.
    */
  struct Node : private NonCopyable<Node>
  {
    //! Create a header
    Node(const NodeType type) noexcept;

    //! Create a header
    Node(ConstKeyT& key, const NodeType type, const std::size_t byte_num) noexcept;

    //! Move data
    Node(Node&& other) noexcept;

    //! Move data
    Node& operator=(Node&& other) noexcept;

    //!
    std::size_t byteNum() const noexcept;

    //!
    NodePtrMutableT* getChild(ConstKeyT& key) noexcept;

    //!
    bool isFull() const noexcept;

    //! Check if the node is full node
    bool isFullNode() const noexcept;

    //! Check if the node is indirect node
    bool isIndirectNode() const noexcept;

    //! Check if the node is leaf node
    bool isLeafNode() const noexcept;

    //! Check if the node is sparse node
    bool isSparseNode() const noexcept;

    //! Return the underlying lock
    flock::Lock& lock() noexcept;

    //! Return the underlying lock
    const flock::Lock& lock() const noexcept;

    KeyT key_;
    NodeType type_;
    flock::WriteOnce<bool> is_removed_;
    uint16b byte_num_;
  };

  /*!
    \brief 256 entries, one for each value of a byte, null if empty

    No detailed description.
    */
  struct FullNode : public Node
  {
    //! Create a node
    FullNode() noexcept;

    //! Move data
    FullNode(FullNode&& other) noexcept;

    //! Move data
    Node& operator=(Node&& other) noexcept;

    //! 
    NodePtrMutableT* getChildImpl(ConstKeyT& key) noexcept;

    //!
    void initChild(ConstKeyT& key, NodePtrT v) noexcept;

    //!
    bool isFullImpl() const noexcept;

    flock::Lock lock_;
    std::array<NodePtrMutableT, 256> children_;
  };

  /*!
    \brief up to 64 entries, with array of 256 1-byte pointers to the 64 entries.

    Entries can only be added, but on deletion the entry pointer can be made null, and later refilled.
    */
  struct IndirectNode : public Node 
  {
    //! Create a node
    IndirectNode() noexcept;

    //! Move data
    IndirectNode(IndirectNode&& other) noexcept;

    //! Move data
    IndirectNode& operator=(IndirectNode&& other) noexcept;

    //!
    bool addChild(ConstKeyT& key, NodePtrT v) noexcept;

    //!
    NodePtrMutableT* getChildImpl(ConstKeyT& key) noexcept;

    //!
    void initChild(ConstKeyT& key, NodePtrT v) noexcept;

    //!
    bool isFullImpl() const noexcept;

    flock::Lock lock_;
    flock::Mutable<int32b> num_used_; //!< Could be aba free
    std::array<flock::WriteOnce<int8b>, 256> index_; //!< -1 means empty
    std::array<NodePtrMutableT, 64> ptr_;
  };

  /*!
    \brief No brief description

    No detailed description.
    */
  struct alignas(64) SparseNode : public Node
  {
    //! Create a node
    SparseNode() noexcept;

    //! Create a node
    SparseNode(const std::size_t byte_num,
               NodePtrT v1,
               ConstKeyT& key1,
               NodePtrT v2,
               ConstKeyT& key2) noexcept;

    //!
    NodePtrMutableT* getChildImpl(ConstKeyT key) noexcept;

    //!
    void initChild(ConstKeyT& key, NodePtrT v) noexcept;

    //!
    bool isFullImpl() const noexcept;

    flock::Lock lock_;
    std::size_t num_used_;
    std::array<uint8b, 16> keys_;
    std::array<NodePtrMutableT, 16> ptr_;
  };

  /*!
    \brief No brief description

    No detailed description.
    */
  struct LeafNode : public Node
  {
    //! Create a node
    LeafNode(RReference value) noexcept;

    //! Move data
    LeafNode(LeafNode&& other) noexcept;

    //! Move data
    LeafNode& operator=(LeafNode&& other) noexcept;

    ValueT value_;
  };

  /*!
    \brief No brief description

    No detailed description.
    */
  struct FindQueryResult
  {
    NodePtrT gp_ = nullptr;
    NodePtrT p_ = nullptr;
    NodePtrMutableT* cptr_ = nullptr;
    NodePtrT c_ = nullptr;
    std::size_t byte_pos_ = 0;
  };


  //! Adds a new child to p
  bool addChild(NodePtrT gp, NodePtrT p, RReference value) noexcept;

  //! Adds a new child to p
  bool addChildImpl(ConstKeyT& key,
                    NodePtrT p,
                    NodePtrMutableT* x,
                    Reference value) noexcept;

  //! Insert the given value into the bst
  template <typename ...Args>
  [[nodiscard]] std::optional<size_type> addImpl(Args&&... args);

  //! Compare the two keys
//  static bool compare(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept;
//
//  //! Compare the two keys of nodes
//  static bool compareNode(ConstStoragePtr lhs, ConstStoragePtr rhs) noexcept;
//
//  //! Check if the two given keys are same
//  static bool equal(ConstStoragePtr lhs, ConstKeyT& rhs) noexcept;

  //!
  static FindQueryResult findLocation(NodePtrT root, ConstKeyT& key) noexcept;

  //!
  static std::size_t getByte(ConstKeyT& key, const std::size_t pos) noexcept;

  //! Return the index of the given node
//  size_type getIndex(ConstStoragePtr node) const noexcept;
//
//  //! Return the storage by the given index
//  StorageRef getStorage(const size_type index) noexcept;
//
//  //! Return the storage by the given index
//  ConstStorageRef getStorage(const size_type index) const noexcept;

  //! Return the invalid id
  static constexpr size_type invalidId() noexcept;

  //! Issue a storage index from the index stack
  size_type issueStorageIndex() noexcept;


  flock::MemoryPool<FullNode> full_pool_;
  flock::MemoryPool<IndirectNode> indirect_pool_;
  flock::MemoryPool<SparseNode> sparse_pool_;
  flock::MemoryPool<LeafNode> leaf_pool_;
  flock::Epoch epoch_;
};

} // namespace zisc

#include "lock_free_lock_art-inl.hpp"

#endif // ZISC_LOCK_FREE_LOCK_ART_HPP
