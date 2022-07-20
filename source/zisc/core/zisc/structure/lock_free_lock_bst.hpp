/*!
  \file lock_free_lock_bst.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_LOCK_BST_HPP
#define ZISC_LOCK_FREE_LOCK_BST_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "map.hpp"
#include "zisc/boolean.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/internal/flock/descriptor.hpp"
#include "zisc/internal/flock/epoch.hpp"
#include "zisc/internal/flock/epoch_pool_impl.hpp"
#include "zisc/internal/flock/lock.hpp"
#include "zisc/internal/flock/log.hpp"
#include "zisc/internal/flock/log_array.hpp"
#include "zisc/internal/flock/memory_pool.hpp"
#include "zisc/internal/flock/mutable.hpp"
#include "zisc/internal/flock/tagged_pool_impl.hpp"
#include "zisc/internal/flock/write_announcements.hpp"
#include "zisc/internal/flock/write_once.hpp"
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
class LockFreeLockBst : public Map<LockFreeLockBst<Key, T, Compare>, Key, T, Compare>
{
 public:
  // Type aliases
  using BaseMapT = Map<LockFreeLockBst<Key, T, Compare>, Key, T, Compare>;
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
  LockFreeLockBst(const std::span<const std::thread::id> thread_id_list,
                  pmr::memory_resource* mem_resource) noexcept;

  //! Create a bst
  LockFreeLockBst(const std::span<const std::thread::id> thread_id_list,
                  const size_type cap,
                  pmr::memory_resource* memory_resource) noexcept;

  //! Move a data
  LockFreeLockBst(LockFreeLockBst&& other) noexcept;

  //! Destroy the bst
  ~LockFreeLockBst() noexcept;


  //! Move a data
  LockFreeLockBst& operator=(LockFreeLockBst&& other) noexcept;


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
  // Type aliases
  using TimestampT = flock::Epoch::TimestampImplT;
  struct Node;
  using ConstNodeT = std::add_const_t<Node>;
  using NodePtrT = std::add_pointer_t<Node>;
  using ConstNodePtrT = std::add_pointer_t<ConstNodeT>;
  using NodePtrMutableT = flock::Mutable<NodePtrT>;
  using LockT = flock::Lock<false>;

  /*!
    \brief Common header for internal nodes and leaves

    No detailed description.
    */
  struct Node : private NonCopyable<Node>
  {
    //! Create a node
    Node() noexcept = default;

    //! Move data
    Node(Node&& other) noexcept;

    //! Destroy the node
    virtual ~Node() noexcept = default;

    //! Move data
    Node& operator=(Node&& other) noexcept;

    //! Check if the node is leaf
    virtual bool isLeaf() const noexcept = 0;

    //! Return the underlying key
    virtual ConstKeyT& key() const noexcept = 0;

    //! Return the underlying left child
    virtual NodePtrT leftChild(flock::Log* log) noexcept;

    //! Return the underlying left child
    virtual ConstNodePtrT leftChild(flock::Log* log) const noexcept;

    //! Return the underlying lock
    virtual LockT* lock() noexcept;

    //! Return the underlying lock
    virtual const LockT* lock() const noexcept;

    //! Return the underlying right child
    virtual NodePtrT rightChild(flock::Log* log) noexcept;

    //! Return the underlying right child
    virtual ConstNodePtrT rightChild(flock::Log* log) const noexcept;
  };

  /*!
    \brief No brief description

    No detailed description.
    */
  struct InternalNode : public Node
  {
    //! Create a node
    InternalNode(ConstKeyT& key, NodePtrT left, NodePtrT right) noexcept;

    //! Move data
    InternalNode(InternalNode&& other) noexcept;

    //! Destroyt the node
    ~InternalNode() noexcept override = default;

    //! Move data
    InternalNode& operator=(InternalNode&& other) noexcept;

    //! Check if the node is leaf
    bool isLeaf() const noexcept override;

    //! Return the underlying key
    ConstKeyT& key() const noexcept override;

    //! Return the underlying left child
    NodePtrT leftChild(flock::Log* log) noexcept override;

    //! Return the underlying left child
    ConstNodePtrT leftChild(flock::Log* log) const noexcept override;

    //! Return the underlying lock
    LockT* lock() noexcept override;

    //! Return the underlying lock
    const LockT* lock() const noexcept override;

    //! Return the underlying right child
    NodePtrT rightChild(flock::Log* log) noexcept override;

    //! Return the underlying right child
    ConstNodePtrT rightChild(flock::Log* log) const noexcept override;


    LockT lock_;
    NodePtrMutableT left_;
    NodePtrMutableT right_;
    KeyT key_;
    flock::WriteOnce<bool> is_removed_; //!< Not used for leaves, but fits here
  };

  /*!
    \brief No brief description

    No detailed description.
    */
  struct DummyNode : public Node
  {
    //! Create a node
    DummyNode(ConstKeyT& key) noexcept;

    //! Destroy the node
    ~DummyNode() noexcept override = default;

    //! Check if the node is leaf
    bool isLeaf() const noexcept override;

    //! Return the underlying key
    ConstKeyT& key() const noexcept override;

    KeyT key_;
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

    //! Destroy the node
    ~LeafNode() noexcept override = default;

    //! Move data
    LeafNode& operator=(LeafNode&& other) noexcept;

    //! Check if the node is leaf
    bool isLeaf() const noexcept override;

    //! Return the underlying key
    ConstKeyT& key() const noexcept override;

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
    NodePtrT c_ = nullptr;
    bool is_gp_left_ = false;
    bool is_p_left_ = false;
    [[maybe_unused]] Padding<std::alignment_of_v<NodePtrT> - 2> pad_;
  };


  //! Insert the given value into the bst
  [[nodiscard]] std::optional<size_type> addImpl(Reference value,
                                                 FindQueryResult& query) noexcept;

  //! Compare the two keys
  static bool compare(ConstKeyT& lhs, ConstKeyT& rhs) noexcept;

  //! Compare the two keys
  static bool compare(ConstKeyT& lhs, ConstNodePtrT rhs) noexcept;

  //! Check if the two given keys are same
  static bool equal(ConstKeyT& lhs, ConstKeyT& rhs) noexcept;

  //! Check if the two given keys are same
  static bool equal(ConstKeyT& lhs, ConstNodePtrT rhs) noexcept;

  //!
  FindQueryResult findLocation(NodePtrT root, ConstKeyT& key) const noexcept;

  //! Find the minimum key in the bst
  std::optional<Pointer> findMinKeyImpl() const noexcept;

  //! Initialize the node
  void initialize() noexcept;

  //! Return the invalid id
  static constexpr size_type invalidId() noexcept;

  //! Return the maximym key
  static constexpr KeyT maxKey() noexcept;

  //! Return the overflow id
  static constexpr size_type overflowId() noexcept;

  //! Remove the value from the bst
  [[nodiscard]] std::optional<size_type> removeImpl(FindQueryResult& query) noexcept;

  //! Remove nodes under the subtree
  void removeSubTree(NodePtrT root) noexcept;

  //! Return the underlying root node
  NodePtrT root() noexcept;

  //! Return the underlying root node
  ConstNodePtrT root() const noexcept;

  //!
  flock::Log& threadLog() const noexcept;

  //!
  template <std::invocable Thank>
  std::invoke_result_t<Thank> tryLock(NodePtrT node, Thank&& func) noexcept;


  pmr::unique_ptr<TimestampT> timestamp_;
  pmr::unique_ptr<flock::Epoch> epoch_;
  flock::WriteAnnouncements write_announcements_;
  pmr::vector<flock::Log> log_list_;
  pmr::vector<std::size_t> current_id_list_;
  pmr::vector<Boolean> helping_list_;
  flock::EpochPoolImpl<flock::LogArray> log_array_pool_;
  flock::MemoryPool<InternalNode> internal_pool_;
  flock::MemoryPool<LeafNode> leaf_pool_;
  flock::MemoryPool<flock::Descriptor,
                    flock::TaggedPoolImpl<flock::Descriptor>> descriptor_pool_;
  pmr::unique_ptr<DummyNode> dummy_;
  pmr::unique_ptr<InternalNode> root_;
};

} // namespace zisc

#include "lock_free_lock_bst-inl.hpp"

#endif // ZISC_LOCK_FREE_LOCK_BST_HPP
