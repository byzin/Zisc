/*!
  \file tagged_pool_impl-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_TAGGED_POOL_IMPL_INL_HPP
#define ZISC_FLOCK_TAGGED_POOL_IMPL_INL_HPP

#include "tagged_pool_impl.hpp"
// Standard C++ library
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "epoch_pool_impl.hpp"
#include "worker_info.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \param [in] epoch No description.
  \param [in,out] mem_resource No description.
  */
template <typename xT> inline
TaggedPoolImpl<xT>::TaggedPoolImpl(Epoch* epoch, pmr::memory_resource* mem_resource) noexcept :
    pool_impl_{epoch, mem_resource}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename xT> inline
TaggedPoolImpl<xT>::TaggedPoolImpl(TaggedPoolImpl&& other) noexcept :
    pool_impl_{std::move(other.pool_impl_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename xT> inline
auto TaggedPoolImpl<xT>::operator=(TaggedPoolImpl&& other) noexcept -> TaggedPoolImpl&
{
  pool_impl_ = std::move(other.pool_impl_);
  return *this;
}

/*!
  \details No detailed description

  \param [in,out] p No description.
  */
template <typename xT> inline
void TaggedPoolImpl<xT>::acquire(Pointer p) noexcept
{
  p->acquired().test_and_set(std::memory_order::release);
}

/*!
  \details No detailed description
  */
template <typename xT> inline
void TaggedPoolImpl<xT>::clear() noexcept
{
  pool_impl_.clear();
}

/*!
  \details No detailed description

  \param [in,out] p No description.
  */
template <typename xT> inline
void TaggedPoolImpl<xT>::destruct(Pointer p) noexcept
{
  pool_impl_.destruct(p);
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename xT> template <typename ...Args> inline
auto TaggedPoolImpl<xT>::newObj(Args&&... args) noexcept -> Pointer
{
  return pool_impl_.newObj(std::forward<Args>(args)...);
}

/*!
  \details No detailed description

  \param [in] size No description.
  */
template <typename xT> inline
void TaggedPoolImpl<xT>::reserve(const std::size_t size) noexcept
{
  pool_impl_.reserve(size);
}

/*!
  \details No detailed description

  \param [in] p No description.
  */
template <typename xT> inline
void TaggedPoolImpl<xT>::retire(Pointer p) noexcept
{
  const bool x = p->acquired().test(std::memory_order::acquire);
  if (!x) {
    destruct(p);
  }
  else {
    p->acquired().clear(std::memory_order::release);
    pool_impl_.retire(p);
  }
}

/*!
  \details No detailed description

  \param [in] info No description.
  */
template <typename xT> inline
void TaggedPoolImpl<xT>::setWorkerInfo(const WorkerInfo& info) noexcept
{
  pool_impl_.setWorkerInfo(info);
}

/*!
  \details No detailed description

  \param [in] n No description.
  */
template <typename xT> inline
void TaggedPoolImpl<xT>::shuffle(const std::size_t n) noexcept
{
  pool_impl_.shuffle(n);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename xT> inline
const WorkerInfo& TaggedPoolImpl<xT>::workerInfo() const noexcept
{
  return pool_impl_.workerInfo();
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_TAGGED_POOL_IMPL_INL_HPP */
