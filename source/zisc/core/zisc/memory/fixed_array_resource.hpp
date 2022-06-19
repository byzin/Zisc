/*!
  \file fixed_array_resource.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FIXED_ARRAY_RESOURCE_HPP
#define ZISC_FIXED_ARRAY_RESOURCE_HPP

// Standard C++ library
#include <atomic>
#include <bit>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>
// Zisc
#include "memory.hpp"
#include "std_memory_resource.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/concurrency/bitset.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  */
template <typename Type>
class FixedArrayResource : public pmr::memory_resource,
                           private NonCopyable<FixedArrayResource<Type>>
{
 public:
  // Type aliases
  using BadAlloc = Memory::BadAlloc;


  //! Create a resource
  FixedArrayResource(pmr::memory_resource* mem_resource) noexcept;

  //! Create a resource
  FixedArrayResource(FixedArrayResource&& other) noexcept;

  //! Destroy the resource
  ~FixedArrayResource() noexcept override = default;


  //! Create a resource
  FixedArrayResource& operator=(FixedArrayResource&& other) noexcept;


  //! Return the maximum available alignment per allocation
  static constexpr std::size_t alignmentMax() noexcept;

  //! Allocate memory
  void* allocateMemory(const std::size_t size,
                       const std::size_t alignment);

  //! Return the number of used storage in the fixed array
  std::size_t count() const noexcept;

  //! Return the maximum number of available storage in the fixed array
  std::size_t countMax() const noexcept;

  //! Clear the usage status in the resource
  void clear() noexcept;

  //! Deallocate memory
  void deallocateMemory(void* data,
                        const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Set the count max of the resource
  void setCountMax(const std::size_t c) noexcept;

  //! Return the maximum available size per allocation
  static constexpr std::size_t sizeMax() noexcept;

 private:
  // Type aliases
  using StorageT = std::aligned_storage_t<sizeof(Type), std::alignment_of_v<Type>>;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
  static_assert(std::has_single_bit(kCacheLineSize));


  //! Allocate memory
  void* do_allocate(std::size_t size,
                    std::size_t alignment) override;

  //! Deallocate memory
  void do_deallocate(void* data,
                     std::size_t size,
                     std::size_t alignment) override;

  //! Compare for equality with another memory resource
  bool do_is_equal(const pmr::memory_resource& other) const noexcept override;

  //! Find and get a ownership of a free storage
  std::size_t findAndGetOwnership(const std::size_t start) noexcept;

  //! Return the invalid index
  static constexpr std::size_t invalidIndex() noexcept;

  //! Remap index in order to avoid contentions
  std::size_t permuteIndex(const std::size_t index) const noexcept;


  alignas(kCacheLineSize) std::atomic_size_t count_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(count_)> pad1_;
  pmr::vector<StorageT> storage_list_;
  Bitset used_list_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(storage_list_) - sizeof(used_list_)> pad2_;
};

} /* namespace zisc */

#include "fixed_array_resource-inl.hpp"

#endif /* ZISC_FIXED_ARRAY_RESOURCE_HPP */
