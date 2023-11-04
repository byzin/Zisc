/*!
  \file fixed_array_resource.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
  using ValueT = Type;
  using ConstValueT = std::add_const_t<ValueT>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_pointer_t<ConstValueT>;
  using BadAllocT = Memory::BadAllocation;


  //! Create a resource
  explicit FixedArrayResource(pmr::memory_resource* mem_resource) noexcept;

  //! Create a resource
  FixedArrayResource(FixedArrayResource&& other) noexcept;

  //! Destroy the resource
  ~FixedArrayResource() noexcept override = default;


  //! Create a resource
  auto operator=(FixedArrayResource&& other) noexcept -> FixedArrayResource&;


  //! Return the maximum available alignment per allocation
  static constexpr auto alignmentMax() noexcept -> std::size_t;

  //! Allocate memory
  [[nodiscard]]
  auto allocateMemory(const std::size_t size,
                      const std::size_t alignment) -> void*;

  //! Return the number of used storage in the fixed array
  auto count() const noexcept -> std::size_t;

  //! Return the maximum number of available storage in the fixed array
  auto countMax() const noexcept -> std::size_t;

  //! Clear the usage status in the resource
  void clear() noexcept;

  //! Return the underlying data pointer
  auto data() noexcept -> Pointer;

  //! Return the underlying data pointer
  auto data() const noexcept -> ConstPointer;

  //! Deallocate memory
  void deallocateMemory(void* data,
                        const std::size_t size,
                        const std::size_t alignment) noexcept;

  //! Set the count max of the resource
  void setCountMax(const std::size_t c);

  //! Return the maximum available size per allocation
  static constexpr auto sizeMax() noexcept -> std::size_t;

 private:
  // Type aliases
  using StorageT = std::aligned_storage_t<sizeof(Type), alignof(Type)>;


  static constexpr std::size_t kCacheLineSize = Config::l1CacheLineSize();
  static_assert(std::has_single_bit(kCacheLineSize));


  //! Allocate memory
  [[nodiscard]]
  auto do_allocate(std::size_t size,
                   std::size_t alignment) -> void* override;

  //! Deallocate memory
  void do_deallocate(void* data,
                     std::size_t size,
                     std::size_t alignment) override;

  //! Compare for equality with another memory resource
  auto do_is_equal(const pmr::memory_resource& other) const noexcept -> bool override;

  //! Find and get a ownership of a free storage
  auto findAndGetOwnership(const std::size_t start) noexcept -> std::size_t;

  //! Return the invalid index
  static constexpr auto invalidIndex() noexcept -> std::size_t;

  //! Remap index in order to avoid contentions
  auto permuteIndex(const std::size_t index) const noexcept -> std::size_t;


  alignas(kCacheLineSize) std::atomic_size_t count_;
  [[maybe_unused]] Padding<kCacheLineSize - sizeof(count_)> pad1_{};
  pmr::vector<StorageT> storage_list_;
  Bitset used_list_;
  [[maybe_unused]] Padding<kCacheLineSize - (sizeof(storage_list_) + sizeof(used_list_))> pad2_{};
};

} /* namespace zisc */

#include "fixed_array_resource-inl.hpp"

#endif /* ZISC_FIXED_ARRAY_RESOURCE_HPP */
