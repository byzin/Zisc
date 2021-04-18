/*!
  \file help_optimal_bst_node_inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_HELP_OPTIMAL_BST_NODE_INL_HPP
#define ZISC_HELP_OPTIMAL_BST_NODE_INL_HPP

#include "help_optimal_bst_node.hpp"
// Standard C++ library
#include <atomic>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "zisc/bit.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
inline
HelpOptimalBstNode::HelpOptimalBstNode() noexcept : key_{invalidKey()}
{
}

/*!
  \details No detailed description
  */
inline
HelpOptimalBstNode::~HelpOptimalBstNode() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
HelpOptimalBstNode::HelpOptimalBstNode(HelpOptimalBstNode&& other) noexcept :
    key_{other.key()},
    left_child_{other.leftChild().load(std::memory_order::acquire)},
    right_child_{other.rightChild().load(std::memory_order::acquire)},
    backtrack_{other.backtrack().load(std::memory_order::acquire)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
HelpOptimalBstNode& HelpOptimalBstNode::operator=(HelpOptimalBstNode&& other) noexcept
{
  key_ = other.key();
  left_child_ = other.leftChild().load(std::memory_order::acquire);
  right_child_ = other.rightChild().load(std::memory_order::acquire);
  backtrack_ = other.backtrack().load(std::memory_order::acquire);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<std::size_t>& HelpOptimalBstNode::backtrack() noexcept
{
  return backtrack_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<std::size_t>& HelpOptimalBstNode::backtrack() const noexcept
{
  return backtrack_;
}

/*!
  \details No detailed description

  \param [in] is_left No description.
  \return No description
  */
inline
std::atomic<std::size_t>& HelpOptimalBstNode::child(const bool is_left) noexcept
{
  return is_left ? leftChild() : rightChild();
}

/*!
  \details No detailed description

  \param [in] is_left No description.
  \return No description
  */
inline
const std::atomic<std::size_t>& HelpOptimalBstNode::child(const bool is_left) const noexcept
{
  return is_left ? leftChild() : rightChild();
}

/*!
  \details No detailed description
  */
inline
void HelpOptimalBstNode::clear() noexcept
{
  setKey(invalidKey());
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr double HelpOptimalBstNode::invalidKey() noexcept
{
  const double k = std::numeric_limits<double>::infinity();
  return k;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool HelpOptimalBstNode::isSplice() const noexcept
{
  const bool result = key() == min0Key();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool HelpOptimalBstNode::isValid() const noexcept
{
  const bool result = key() != invalidKey();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
double HelpOptimalBstNode::key() const noexcept
{
  return key_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<std::size_t>& HelpOptimalBstNode::leftChild() noexcept
{
  return left_child_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<std::size_t>& HelpOptimalBstNode::leftChild() const noexcept
{
  return left_child_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr double HelpOptimalBstNode::max0Key() noexcept
{
  const double k = (std::numeric_limits<double>::max)();
  return k;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr double HelpOptimalBstNode::max1Key() noexcept
{
  static_assert(sizeof(double) == sizeof(uint64b));
  const uint64b v = bit_cast<uint64b>(max0Key()) - 1;
  const double k = bit_cast<double>(v);
  return k;
}

static_assert(HelpOptimalBstNode::max1Key() < HelpOptimalBstNode::max0Key(),
              "max1Key isn't less than max0Key.");

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr double HelpOptimalBstNode::max2Key() noexcept
{
  static_assert(sizeof(double) == sizeof(uint64b));
  const uint64b v = bit_cast<uint64b>(max1Key()) - 1;
  const double k = bit_cast<double>(v);
  return k;
}

static_assert(HelpOptimalBstNode::max2Key() < HelpOptimalBstNode::max1Key(),
              "max2Key isn't less than max1Key.");

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr double HelpOptimalBstNode::min0Key() noexcept
{
  const double k = std::numeric_limits<double>::lowest();
  return k;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr double HelpOptimalBstNode::min1Key() noexcept
{
  static_assert(sizeof(double) == sizeof(uint64b));
  const uint64b v = bit_cast<uint64b>(min0Key()) - 1;
  const double k = bit_cast<double>(v);
  return k;
}

static_assert(HelpOptimalBstNode::min0Key() < HelpOptimalBstNode::min1Key(),
              "min1Key isn't greater than min0Key.");

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<std::size_t>& HelpOptimalBstNode::rightChild() noexcept
{
  return right_child_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<std::size_t>& HelpOptimalBstNode::rightChild() const noexcept
{
  return right_child_;
}

/*!
  \details No detailed description

  \param [in] k No description.
  */
inline
void HelpOptimalBstNode::setKey(const double k) noexcept
{
  key_ = k;
}

} // namespace zisc

#endif // ZISC_HELP_OPTIMAL_BST_NODE_INL_HPP
