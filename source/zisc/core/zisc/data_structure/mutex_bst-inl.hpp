/*!
  \file mutex_bst-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_BST_INL_HPP
#define ZISC_MUTEX_BST_INL_HPP

#include "mutex_bst.hpp"
// Standard C++ library
#include <algorithm>
#include <cstddef>
#include <limits>
#include <mutex>
#include <numeric>
#include <shared_mutex>
#include <tuple>
#include <utility>
#include <vector>
// Zisc
#include "bounded_bst.hpp"
#include "mutex_bst_node.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
MutexBst::MutexBst(pmr::memory_resource* mem_resource) noexcept :
    MutexBst(defaultCapacity(), mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
inline
MutexBst::MutexBst(const size_type cap,
                   pmr::memory_resource* mem_resource) noexcept :
    node_list_{typename decltype(node_list_)::allocator_type{mem_resource}},
    index_stack_{typename decltype(index_stack_)::allocator_type{mem_resource}}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
inline
MutexBst::MutexBst(MutexBst&& other) noexcept :
    node_list_{std::move(other.node_list_)},
    index_stack_{std::move(other.index_stack_)}
{
}

/*!
  \details No detailed description
  */
inline
MutexBst::~MutexBst() noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
inline
MutexBst& MutexBst::operator=(MutexBst&& other) noexcept
{
  node_list_ = std::move(other.node_list_);
  index_stack_ = std::move(other.index_stack_);
  return *this;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <ConvertibleTo<double> Type> inline
auto MutexBst::add(const Type& value) -> std::tuple<bool, size_type>
{
  const double key = cast<double>(value);
  size_type node_index = invalidId();
  {
    std::unique_lock<std::shared_mutex> lock{mutex_};
    auto position = std::lower_bound(node_list_.begin(),
                                     node_list_.end(),
                                     key,
                                     MutexBst::compare);
    if ((position == node_list_.end()) || !equal(position->key(), key)) {
      node_index = index_stack_.back();
      index_stack_.pop_back();
      node_list_.emplace(position, key, node_index);
    }
  }
  const bool is_added = node_index != invalidId();
  return std::make_tuple(is_added, node_index);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto MutexBst::capacity() const noexcept -> size_type
{
  size_type cap = 0;
  {
    std::shared_lock<std::shared_mutex> lock{mutex_};
    cap = node_list_.capacity();
  }
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto MutexBst::capacityMax() noexcept -> size_type
{
  const size_type cap = (std::numeric_limits<size_type>::max)();
  return cap;
}

/*!
  \details No detailed description
  */
inline
void MutexBst::clear() noexcept
{
  node_list_.clear();
  std::iota(index_stack_.rbegin(), index_stack_.rend(), 0);
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <ConvertibleTo<double> Type> inline
auto MutexBst::contain(const Type& value) const noexcept
    -> std::tuple<bool, size_type>
{
  const double key = cast<double>(value);
  bool is_found = false;
  size_type node_index = invalidId();
  {
    std::shared_lock<std::shared_mutex> lock{mutex_};
    auto position = std::lower_bound(node_list_.begin(),
                                     node_list_.end(),
                                     key,
                                     MutexBst::compare);
    is_found = (position != node_list_.end()) && equal(position->key(), key);
    node_index = is_found ? position->index() : invalidId();
  }
  return std::make_tuple(is_found, node_index);
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto MutexBst::defaultCapacity() noexcept -> size_type
{
  const size_type cap = 1024;
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
inline
double MutexBst::findMinKey() const noexcept
{
  double key = 0.0;
  {
    std::shared_lock<std::shared_mutex> lock{mutex_};
    key = node_list_[0].key();
  }
  return key;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <ConvertibleTo<double> Type> inline
auto MutexBst::remove(const Type& value) -> std::tuple<bool, size_type>
{
  const double key = cast<double>(value);
  bool is_removed = false;
  size_type node_index = invalidId();
  {
    std::unique_lock<std::shared_mutex> lock{mutex_};
    auto position = std::lower_bound(node_list_.begin(),
                                     node_list_.end(),
                                     key,
                                     MutexBst::compare);
    if ((position != node_list_.end()) && equal(position->key(), key)) {
      index_stack_.emplace_back(position->index());
      node_index = position->index();
      node_list_.erase(position);
      is_removed = true;
    }
  }
  return std::make_tuple(is_removed, node_index);
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
inline
void MutexBst::setCapacity(const size_type cap) noexcept
{
  node_list_.reserve(cap);
  index_stack_.resize(capacity());
  clear();
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto MutexBst::size() const noexcept -> size_type
{
  size_type s = 0;
  {
    std::shared_lock<std::shared_mutex> lock{mutex_};
    s = node_list_.size();
  }
  return s;
}

/*!
  \details No detailed description

  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
inline
bool MutexBst::compare(const Node& lhs, const double rhs) noexcept
{
  const bool result = lhs.key() < rhs;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto MutexBst::invalidId() noexcept -> size_type
{
  const size_type index = (std::numeric_limits<size_type>::max)();
  return index;
}

} // namespace zisc

#endif // ZISC_MUTEX_BST_INL_HPP
