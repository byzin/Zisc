/*!
  \file mutex_bst_node-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_BST_NODE_INL_HPP
#define ZISC_MUTEX_BST_NODE_INL_HPP

#include "mutex_bst_node.hpp"
// Standard C++ library
#include <cstddef>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] key No description.
  \param [in] index No description.
  */
inline
MutexBstNode::MutexBstNode(const double key, const std::size_t index) noexcept :
    key_{key},
    index_{index}
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
inline
MutexBstNode::MutexBstNode(MutexBstNode&& other) noexcept :
    key_{other.key()},
    index_{other.index()}
{
}

/*!
  \details No detailed description
  */
inline
MutexBstNode::~MutexBstNode() noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
inline
MutexBstNode& MutexBstNode::operator=(MutexBstNode&& other) noexcept
{
  key_ = other.key();
  index_ = other.index();
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
inline
double MutexBstNode::key() const noexcept
{
  return key_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t MutexBstNode::index() const noexcept
{
  return index_;
}

} // namespace zisc

#endif // ZISC_MUTEX_BST_NODE_INL_HPP
