/*!
  \file mutex_bst_node.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_BST_NODE_HPP
#define ZISC_MUTEX_BST_NODE_HPP

// Standard C++ library
#include <cstddef>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class MutexBstNode : private NonCopyable<MutexBstNode>
{
 public:
  //! Create a node
  MutexBstNode(const double key, const std::size_t index) noexcept;

  //! Move data
  MutexBstNode(MutexBstNode&& other) noexcept;

  //! Destroy a node
  ~MutexBstNode() noexcept;


  //! Move data
  MutexBstNode& operator=(MutexBstNode&& other) noexcept;


  //! Return the underlying key
  double key() const noexcept;

  //! Return the underlying index
  std::size_t index() const noexcept;

 private:
  double key_;
  std::size_t index_;
};

} // namespace zisc

#include "mutex_bst_node-inl.hpp"

#endif // ZISC_MUTEX_BST_NODE_HPP
