/*!
  \file lock_free_bst.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \note No notation.
  \attention No attention.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_BST_HPP
#define ZISC_LOCK_FREE_BST_HPP

// Standard C++ library
#include <array>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "map.hpp"
#include "ring_buffer.hpp"
#include "zisc/non_copyable.hpp"
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
template <std::convertible_to<double> Key,
          MappedValue T = void,
          std::invocable<Key, Key> Compare = std::less<Key>>
class LockFreeBst : public Map<LockFreeBst<Key, T, Compare>, Key, T, Compare>
{
 public:
  // Type aliases
  using BaseMapT = Map<LockFreeBst<Key, T, Compare>, Key, T, Compare>;
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
  LockFreeBst(pmr::memory_resource* mem_resource) noexcept;

  //! Create a bst
  LockFreeBst(const size_type cap, pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  LockFreeBst(LockFreeBst&& other) noexcept;

  //! Destroy the bst
  ~LockFreeBst() noexcept;


  //! Move a data
  LockFreeBst& operator=(LockFreeBst&& other) noexcept;


  //! Insert the given value into the bst
  template <typename ...Args>
  [[nodiscard]] std::optional<size_type> add(Args&&... args);

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
  class Node;
  // Type aliases
  using StorageT = DataStorage<ValueT>;
  using ConstStorageT = std::add_const_t<StorageT>;
  using StorageRef = std::add_lvalue_reference_t<StorageT>;
  using ConstStorageRef = std::add_lvalue_reference_t<ConstStorageT>;
  using StoragePtr = std::add_pointer_t<StorageT>;
  using ConstStoragePtr = std::add_pointer_t<ConstStorageT>;
  using ConstNodeT = std::add_const_t<Node>;
  using NodePtr = std::add_pointer_t<Node>;
  using ConstNodePtr = std::add_pointer_t<ConstNodeT>;
  using NodeReference = std::add_lvalue_reference_t<Node>;
  using ConstNodeReference = std::add_lvalue_reference_t<ConstNodeT>;


  enum class ComparisonResult : size_type
  {
    kIsLess,
    kIsGreater,
    kIsEqual
  };

  /*!
    \brief No brief description

    No detailed description.
    */
  class NodeInfo
  {
   public:
    //! Create an info
    NodeInfo() noexcept;

    //! Create an info
    NodeInfo(const size_type index) noexcept;

    //! Create an info
    NodeInfo(const size_type index,
             const bool flag,
             const bool is_marked,
             const bool is_threaded) noexcept;

    //! Create an info
    NodeInfo(const NodeInfo info,
             const bool flag,
             const bool is_marked) noexcept;

    //!
    ~NodeInfo() noexcept = default;


    //! Return the underlying flag
    bool flag() const noexcept;

    //! Return the underlying node pointer
    size_type index() const noexcept;

    //!
    static constexpr size_type indexMax() noexcept;

    //!
    static constexpr size_type invalidIndex() noexcept;

    //!
    bool isInvalid() const noexcept;

    //! Return the underlying mark
    bool isMarked() const noexcept;

    //!
    bool isRoot() const noexcept;

    //! Return the underlying thread flag
    bool isThreaded() const noexcept;

    //!
    static constexpr size_type root0Index() noexcept;

    //!
    static constexpr size_type root1Index() noexcept;

   private:
    //!
    static size_type create(const size_type ptr,
                            const bool flag,
                            const bool is_marked,
                            const bool is_threaded) noexcept;

    //! Return the mask for flag
    static constexpr size_type flagMask() noexcept;

    //! Return the mask for node index
    static constexpr size_type indexMask() noexcept;

    //! Return the mask for mark
    static constexpr size_type markMask() noexcept;

    //! Return the mask for thread
    static constexpr size_type threadMask() noexcept;


    size_type info_ = 0;
  };

  /*!
    \brief No brief description

    No detailed description.
    */
  class Node : private zisc::NonCopyable<Node>
  {
   public:
    //! Create a node
    Node() noexcept;

    //! Move data
    Node(Node&& other) noexcept;

    //! Move data
    Node& operator=(Node&& other) noexcept;

    //!
    std::atomic<NodeInfo>& backLink() noexcept;

    //!
    const std::atomic<NodeInfo>& backLink() const noexcept;

    //!
    NodeInfo backLinkP() const noexcept;

    //!
    std::atomic<NodeInfo>& child(const size_type dir) noexcept;

    //!
    std::atomic<NodeInfo>& child(const ComparisonResult dir) noexcept;

    //!
    const std::atomic<NodeInfo>& child(const size_type dir) const noexcept;

    //!
    const std::atomic<NodeInfo>& child(const ComparisonResult dir) const noexcept;

    //!
    NodeInfo childP(const size_type dir) const noexcept;

    //!
    NodeInfo childP(const ComparisonResult dir) const noexcept;

    //! Return the underlying key
    double key(const bool is_root) const noexcept;

    //! Return the underlying left child
    std::atomic<NodeInfo>& leftChild() noexcept;

    //! Return the underlying left child
    const std::atomic<NodeInfo>& leftChild() const noexcept;

    //! Return the underlying left child
    NodeInfo leftChildP() const noexcept;

    //!
    NodeInfo preLink() const noexcept;

    //! Return the underlying right child
    std::atomic<NodeInfo>& rightChild() noexcept;

    //! Return the underlying right child
    const std::atomic<NodeInfo>& rightChild() const noexcept;

    //! Return the underlying right child
    NodeInfo rightChildP() const noexcept;

    //!
    void setBackLink(NodeInfo info) noexcept;

    //!
    void setChild(const size_type dir, const NodeInfo info) noexcept;

    //!
    void setChild(const ComparisonResult dir, const NodeInfo info) noexcept;

    //!
    void setKey(const double key) noexcept;

    //!
    void setLeftChild(const NodeInfo info) noexcept;

    //!
    void setPreLink(NodeInfo info) noexcept;

    //!
    void setRightChild(const NodeInfo info) noexcept;

    //!
    void setValue(RReference value) noexcept;

    //! Return the underlying value
    Reference value() noexcept;

    //! Return the underlying value
    ConstReference value() const noexcept;

   private:
    std::array<std::atomic<NodeInfo>, 2> child_;
    std::atomic<NodeInfo> back_link_;
    NodeInfo pre_link_;
    union
    {
      StorageT value_;
      double key_;
    };
  };


  //!
  static bool cas(std::atomic<NodeInfo>* old_value,
                  NodeInfo new_value,
                  const NodeInfo replacement) noexcept;

  //!
  void cleanFlag(NodeInfo& prev,
                 NodeInfo& current,
                 NodeInfo& back,
                 const bool is_threaded) noexcept;

  //!
  void cleanFlagNoThreaded(NodeInfo& prev,
                           NodeInfo& current,
                           NodeInfo& back) noexcept;

  //!
  void cleanFlagThreaded(NodeInfo& prev,
                         NodeInfo& current,
                         NodeInfo& back) noexcept;

  //!
  void cleanMark(NodeInfo& current, const size_type mark_dir) noexcept;

  //!
  void cleanMarkLeft(NodeInfo& current) noexcept;

  //!
  void cleanMarkRight(NodeInfo& current) noexcept;

  //!
  static ComparisonResult compare(const double lhs, const double rhs) noexcept;

  //!
  static bool equal(const double lhs, const double rhs) noexcept;

  //!
  size_type getIndex(ConstNodePtr node) const noexcept;

  //!
  NodePtr getNode(const size_type index) noexcept;

  //!
  NodePtr getNode(const NodeInfo info) noexcept;

  //!
  ConstNodePtr getNode(const size_type index) const noexcept;

  //!
  ConstNodePtr getNode(const NodeInfo info) const noexcept;

  //!
  void initialize() noexcept;

  //!
  bool isRoot(const NodeInfo info) const noexcept;

  //!
  bool isRoot(ConstNodePtr node) const noexcept;

  //!
  size_type issueNodeIndex() noexcept;

  //!
  static double keyMinusUlp(ConstKeyT& key) noexcept;

  //!
  size_type locate(NodeInfo& prev,
                   NodeInfo& current,
                   const double key) const noexcept;

  //! Return the number of cas trials
  static constexpr size_type numOfCasTrials() noexcept;

  //!
  void returnNodeIndex(const size_type index) noexcept;

  //!
  bool tryFlag(NodeInfo& prev,
               NodeInfo& current,
               NodeInfo& back,
               const bool is_threaded) noexcept;

  //!
  void tryMark(NodeInfo& current, const size_type dir) noexcept;

  //! Return the invalid id
  static constexpr size_type invalidId() noexcept;


  pmr::vector<Node> node_pool_;
  std::array<Node, 2> root_;
  RingBuffer index_pool_;
};

} /* namespace zisc */

#include "lock_free_bst-inl.hpp"

#endif /* ZISC_LOCK_FREE_BST_HPP */
