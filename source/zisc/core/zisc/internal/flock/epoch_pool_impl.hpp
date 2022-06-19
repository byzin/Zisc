/*!
  \file epoch_pool_impl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_EPOCH_POOL_IMPL_HPP
#define ZISC_FLOCK_EPOCH_POOL_IMPL_HPP

// Standard C++ library
#include <memory>
#include <type_traits>
#include <vector>
// Zisc
#include "epoch.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/fixed_array_resource.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

// Forward declaration
class Epoch;
class WorkerInfo;

/*!
  \brief No brief description

  No detailed description.

  \tparam xT No description.
  */
template <typename xT>
class EpochPoolImpl : private NonCopyable<EpochPoolImpl<xT>>
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

  //!
  struct Link
  {
    Link* next_ = nullptr;
    void* value_ = nullptr;
  };


  //! Create a pool
  EpochPoolImpl(Epoch* epoch, pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  EpochPoolImpl(EpochPoolImpl&& other) noexcept;

  //! Destroy the pool
  ~EpochPoolImpl() noexcept = default;


  //! Move a data
  EpochPoolImpl& operator=(EpochPoolImpl&& other) noexcept;


  //! Noop since epoch announce is used for the whole operation
  void acquire(Pointer p) noexcept;

  //!
  void clear() noexcept;

  //!
  void destruct(Pointer p) noexcept;

  //!
  template <typename ...Args>
  Pointer newObj(Args&&... args) noexcept;

  //!
  void reserve(const std::size_t size) noexcept;

  //!
  void retire(Pointer p) noexcept;

  //! Set the worker info
  void setWorkerInfo(const WorkerInfo& info) noexcept;

  //!
  void shuffle(const std::size_t n) noexcept;

  //! Return the underlying worker info
  const WorkerInfo& workerInfo() const noexcept;

 private:
  //!
  struct alignas(256) OldCurrent
  {
    Link* old_ = nullptr;
    Link* current_ = nullptr;
    Epoch::ValueT epoch_ = 0;
    std::size_t count_ = 0;
  };


  //!
  void clearPool(Link* ptr) noexcept;

  //! Return the underlying epoch
  Epoch& epoch() noexcept;

  //! Return the underlying epoch
  const Epoch& epoch() const noexcept;

  //! Return a list allocator with the underlying list resource
  pmr::polymorphic_allocator<Link> getListAllocator() noexcept;

  //! Return a type allocator with the underlying type resource
  pmr::polymorphic_allocator<ValueT> getTypeAllocator() noexcept;


  pmr::vector<OldCurrent> pool_list_;
  FixedArrayResource<ValueT> type_resource_;
  FixedArrayResource<Link> list_resource_;
  Epoch* epoch_;
  const WorkerInfo* worker_info_;
};

} /* namespace zisc::flock */

#include "epoch_pool_impl-inl.hpp"

#endif /* ZISC_FLOCK_EPOCH_POOL_IMPL_HPP */
