/*!
  \file help_optimal_bst_node.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_HELP_OPTIMAL_BST_NODE_HPP
#define ZISC_HELP_OPTIMAL_BST_NODE_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"

namespace zisc {

/*!
  \brief Represent a node in the bst

  No detailed description.
  */
class HelpOptimalBstNode : private NonCopyable<HelpOptimalBstNode>
{
 public:
  //! Create a empty node
  HelpOptimalBstNode() noexcept;

  //! Destroy the node
  ~HelpOptimalBstNode() noexcept;


  //! Move data
  HelpOptimalBstNode(HelpOptimalBstNode&& other) noexcept;


  //! Move data
  HelpOptimalBstNode& operator=(HelpOptimalBstNode&& other) noexcept;


  //! Return the underlying backtrack index
  std::atomic<std::size_t>& backtrack() noexcept;

  //! Return the underlying backtrack index
  const std::atomic<std::size_t>& backtrack() const noexcept;

  //! Return the child index
  std::atomic<std::size_t>& child(const bool is_left) noexcept;

  //! Return the child index
  const std::atomic<std::size_t>& child(const bool is_left) const noexcept;

  //! Clear the node data
  void clear() noexcept;

  //! Return the invalid constant key
  static constexpr double invalidKey() noexcept;

  //! Check if the node is splice node
  bool isSplice() const noexcept;

  //! Check if the underlying key is valid
  bool isValid() const noexcept;

  //! Return the underlying key
  double key() const noexcept;

  //! Return the left child index of the node
  std::atomic<std::size_t>& leftChild() noexcept;

  //! Return the left child index of the node
  const std::atomic<std::size_t>& leftChild() const noexcept;

  //! Return the maximum key value
  static constexpr double max0Key() noexcept;

  //! Return the maximum key value
  static constexpr double max1Key() noexcept;

  //! Return the maximum key value
  static constexpr double max2Key() noexcept;

  //! Return the minimum key value
  static constexpr double min0Key() noexcept;

  //! Return the minimum key value
  static constexpr double min1Key() noexcept;

  //! Return the right child index of the node
  std::atomic<std::size_t>& rightChild() noexcept;

  //! Return the right child index of the node
  const std::atomic<std::size_t>& rightChild() const noexcept;

  //! Set the key
  void setKey(const double k) noexcept;

 private:
  double key_;
  std::atomic<std::size_t> left_child_;
  std::atomic<std::size_t> right_child_;
  std::atomic<std::size_t> backtrack_;
};

} // namespace zisc

#include "help_optimal_bst_node-inl.hpp"

#endif // ZISC_HELP_OPTIMAL_BST_NODE_HPP
