/*!
  \file tagged_pool_impl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_TAGGED_POOL_IMPL_HPP
#define ZISC_FLOCK_TAGGED_POOL_IMPL_HPP

// Standard C++ library
#include <memory>
#include <type_traits>
// Zisc
#include "epoch_pool_impl.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

// Forward declaration
class Epoch;
class WorkerInfo;

template <typename xT>
class TaggedPoolImpl : private NonCopyable<TaggedPoolImpl<xT>>
{
 public:
  // Type aliases
  using ValueT = xT;
  using ConstValueT = std::add_const_t<ValueT>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_pointer_t<ConstValueT>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_lvalue_reference_t<ConstValueT>;


  //! Create a pool
  TaggedPoolImpl(Epoch* epoch, pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  TaggedPoolImpl(TaggedPoolImpl&& other) noexcept;

  //! Destroy the pool
  ~TaggedPoolImpl() noexcept = default;


  //! Move a data
  TaggedPoolImpl& operator=(TaggedPoolImpl&& other) noexcept;


  //!
  void acquire(Pointer p) noexcept;

  //! Return the capacity of objects in the pool
  std::size_t capacity() const noexcept;

  //! Return the capacity of objects in the pool
  static constexpr std::size_t capacityMax() noexcept;

  //!
  void clear() noexcept;

  //!
  void destruct(Pointer p) noexcept;

  //! Return the underlying epoch
  Epoch& epoch() noexcept;

  //! Return the underlying epoch
  const Epoch& epoch() const noexcept;

  //! Return the index of the given object in the pool
  std::size_t getIndex(ConstReference object) const noexcept;

  //! Return the pointer to an object by the index
  Reference getObject(const std::size_t index) noexcept;

  //! Return the pointer to an object by the index
  ConstReference getObject(const std::size_t index) const noexcept;

  //!
  template <typename ...Args>
  Pointer newObj(Args&&... args) noexcept;

  //!
  void reserve(const std::size_t size) noexcept;

  //!
  void retire(Pointer p) noexcept;

  //!
  void shuffle(const std::size_t n) noexcept;

  //! Return the number of allocated objects in the pool
  std::size_t size() const noexcept;

  //! Return the underlying worker info
  const WorkerInfo& workerInfo() const noexcept;

 private:
  EpochPoolImpl<xT> pool_impl_;
};

} /* namespace zisc::flock */

#include "tagged_pool_impl-inl.hpp"

#endif /* ZISC_FLOCK_TAGGED_POOL_IMPL_HPP */
