/*!
  \file epoch_pool_impl-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_EPOCH_POOL_IMPL_INL_HPP
#define ZISC_FLOCK_EPOCH_POOL_IMPL_INL_HPP

#include "epoch_pool_impl.hpp"
// Standard C++ library
#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "epoch.hpp"
#include "worker_info.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <typename xT> inline
EpochPoolImpl<xT>::EpochPoolImpl(Epoch* epoch,
                                 pmr::memory_resource* mem_resource) noexcept :
    pool_list_{typename decltype(pool_list_)::allocator_type{mem_resource}},
    type_resource_{mem_resource},
    list_resource_{mem_resource},
    epoch_{epoch}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename xT> inline
EpochPoolImpl<xT>::EpochPoolImpl(EpochPoolImpl&& other) noexcept :
    pool_list_{std::move(other.pool_list_)},
    type_resource_{std::move(other.type_resource_)},
    list_resource_{std::move(other.list_resource_)},
    epoch_{other.epoch_},
    worker_info_{other.worker_info_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename xT> inline
auto EpochPoolImpl<xT>::operator=(EpochPoolImpl&& other) noexcept -> EpochPoolImpl&
{
  pool_list_ = std::move(other.pool_list_);
  type_resource_= std::move(other.type_resource_);
  list_resource_ = std::move(other.list_resource_);
  epoch_ = other.epoch_;
  worker_info_ = other.worker_info_;
  return *this;
}

/*!
  \details No detailed description

  \param [in] p No description.
  */
template <typename xT> inline
void EpochPoolImpl<xT>::acquire([[maybe_unused]] Pointer p) noexcept
{
}

/*!
  \details No detailed description
  */
template <typename xT> inline
void EpochPoolImpl<xT>::clear() noexcept
{
  epoch().updateEpoch();
  std::for_each_n(pool_list_.begin(), pool_list_.size(), [this](OldCurrent& p) noexcept
  {
    clearPool(p.old_);
    p.old_ = nullptr;
    clearPool(p.current_);
    p.current_ = nullptr;
  });
  type_resource_.clear();
  list_resource_.clear();
}

/*!
  \details No detailed description

  \param [in] p No description.
  */
template <typename xT> inline
void EpochPoolImpl<xT>::destruct(Pointer p) noexcept
{
  pmr::polymorphic_allocator<ValueT> alloc = getTypeAllocator();
  alloc.template delete_object<ValueT>(p);
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename xT> template <typename ...Args> inline
auto EpochPoolImpl<xT>::newObj(Args&&... args) noexcept -> Pointer
{
  pmr::polymorphic_allocator<ValueT> alloc = getTypeAllocator();
  Pointer v = alloc.template new_object<ValueT>(std::forward<Args>(args)...);
  return v;
}

/*!
  \details No detailed description

  \param [in] size No description.
  */
template <typename xT> inline
void EpochPoolImpl<xT>::reserve(const std::size_t size) noexcept
{
  constexpr std::size_t k = 8 * Config::l1CacheLineSize() * workerInfo().numOfWorkers();
  constexpr std::size_t s = k * size;
  type_resource_.setCountMax(s);
  list_resource_.setCountMax(s);
}

/*!
  \details No detailed description

  \param [in] p No description.
  */
template <typename xT> inline
void EpochPoolImpl<xT>::retire(Pointer p) noexcept
{
  OldCurrent& pool = workerInfo().takeOut(pool_list_);

  if (pool.epoch_ < epoch().getCurrent()) {
    clearPool(pool.old_);
    pool.old_ = pool.current_;
    pool.current_ = nullptr;
    pool.epoch_ = epoch().getCurrent();
  }

  // a heuristic
  if (++pool.count_ % (workerInfo().numOfWorkers() * 10) == 0)
    epoch().updateEpoch();

  {
    pmr::polymorphic_allocator<Link> alloc = getListAllocator();
    Link* link = alloc.template new_object<Link>();
    link->next_ = pool.current_;
    link->value_ = cast<void*>(p);
    pool.current_ = link;
  }
}

/*!
  \details No detailed description

  \param [in] info No description.
  */
template <typename xT> inline
void EpochPoolImpl<xT>::setWorkerInfo(const WorkerInfo& info) noexcept
{
  worker_info_ = &info;
  pool_list_.resize(info.numOfWorkers());
}

/*!
  \details No detailed description

  \return No description
  */
template <typename xT> inline
const WorkerInfo& EpochPoolImpl<xT>::workerInfo() const noexcept
{
  return *worker_info_;
}

/*!
  \details No detailed description

  \param [in] n No description.
  */
template <typename xT> inline
void EpochPoolImpl<xT>::shuffle([[maybe_unused]] const std::size_t n) noexcept
{
}

/*!
  \details No detailed description

  \param [in] ptr No description.
  */
template <typename xT> inline
void EpochPoolImpl<xT>::clearPool(Link* ptr) noexcept
{
  pmr::polymorphic_allocator<Link> alloc = getListAllocator();
  while (ptr != nullptr) {
    Link* tmp = ptr;
    ptr = ptr->next_;
    destruct(reinterp<Pointer>(tmp->value_));
    alloc.template delete_object<Link>(tmp);
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <typename xT> inline
Epoch& EpochPoolImpl<xT>::epoch() noexcept
{
  return *epoch_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename xT> inline
const Epoch& EpochPoolImpl<xT>::epoch() const noexcept
{
  return *epoch_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename xT> inline
auto EpochPoolImpl<xT>::getListAllocator() noexcept -> pmr::polymorphic_allocator<Link>
{
  pmr::polymorphic_allocator<Link> alloc{&list_resource_};
  return alloc;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename xT> inline
auto EpochPoolImpl<xT>::getTypeAllocator() noexcept -> pmr::polymorphic_allocator<ValueT>
{
  pmr::polymorphic_allocator<ValueT> alloc{&type_resource_};
  return alloc;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_EPOCH_POOL_IMPL_INL_HPP */
