/*!
  \file bitset.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BITSET_HPP
#define ZISC_BITSET_HPP

// Standard C++ library
#include <array>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <memory_resource>
#include <type_traits>
#include <vector>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class Bitset : private NonCopyable<Bitset>
{
 public:
  // Types
  using BitT = uint64b;
  using ConstT = std::add_const_t<BitT>;
  using Reference = std::add_lvalue_reference_t<BitT>;
  using ConstReference = std::add_const_t<std::add_lvalue_reference_t<ConstT>>;
  using Pointer = std::add_pointer_t<BitT>;
  using ConstPointer = std::add_const_t<std::add_pointer_t<ConstT>>;


  //! Create a bitset
  explicit Bitset(std::pmr::memory_resource* mem_resource) noexcept;

  //! Create a bitset
  Bitset(const std::size_t s, std::pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  Bitset(Bitset&& other) noexcept;


  //! Move a data
  auto operator=(Bitset&& other) noexcept -> Bitset&;

  //! Access the bit at position pos
  auto operator[](const std::size_t pos) const noexcept -> bool;


  //! Return the bit size of a block
  static constexpr auto blockBitSize() noexcept -> std::size_t;

  //! Return the bit size of a chunk
  static constexpr auto chunkBitSize() noexcept -> std::size_t;

  //! Return the number of bits set to true
  [[nodiscard]]
  auto count() const noexcept -> std::size_t;

  //! Return the number of bits set to true
  [[nodiscard]]
  auto count(const std::size_t begin, const std::size_t end) const noexcept -> std::size_t;

  //! Return the bits of the block at the pos
  [[nodiscard]]
  auto getBlockBits(const std::size_t pos) const noexcept -> BitT;

  //! Check if all bits are set to true
  [[nodiscard]]
  auto isAll() const noexcept -> bool;

  //! Check if all bits are set to true
  [[nodiscard]]
  auto isAll(const std::size_t begin, const std::size_t end) const noexcept -> bool;

  //! Check if any of the bits are set to true
  [[nodiscard]]
  auto isAny() const noexcept -> bool;

  //! Check if any of the bits are set to true
  [[nodiscard]]
  auto isAny(const std::size_t begin, const std::size_t end) const noexcept -> bool;

  //! Check if none of the bits are set to true
  [[nodiscard]]
  auto isNone() const noexcept -> bool;

  //! Check if none of the bits are set to true
  [[nodiscard]]
  auto isNone(const std::size_t begin, const std::size_t end) const noexcept -> bool;

  //! Set bits to the given value
  void reset(const bool value = false) noexcept;

  //! Set bits to the given value
  void reset(const std::size_t begin, const std::size_t end, const bool value = false) noexcept;

  //! Set the number of bits
  void setSize(const std::size_t s);

  //! Return the number of bits
  [[nodiscard]]
  auto size() const noexcept -> std::size_t;

  //! Return the value at the given position
  [[nodiscard]]
  auto test(const std::size_t pos) const noexcept -> bool;

  //! Atomically set value and obtain it's previous value
  auto testAndSet(const std::size_t pos, const bool value) noexcept -> bool;

 private:
  // Type aliases
  using AtomicT = std::atomic<BitT>; //!< The type of block in a chunk
  using AConstT = std::add_const_t<AtomicT>;
  using AReference = std::add_lvalue_reference_t<AtomicT>;
  using AConstReference = std::add_lvalue_reference_t<AConstT>;
  using APointer = std::add_pointer_t<AtomicT>;
  using AConstPointer = std::add_pointer_t<AConstT>;


  static constexpr std::size_t kChunkAlignment = 2 * Config::l1CacheLineSize();


  //! Represent a block in the bitset
  class alignas(kChunkAlignment) Chunk
  {
   public:
    //! Create a chunk
    Chunk() noexcept = default;

    //! Move a data
    Chunk(Chunk&& other) noexcept;

    //! Move a data
    auto operator=(Chunk&& other) noexcept -> Chunk&;

    //! Copy the bits
    void set(const Chunk& other) noexcept;

    static_assert(kChunkAlignment % sizeof(AtomicT) == 0);
    static constexpr std::size_t kN = kChunkAlignment / sizeof(AtomicT);

    //!
    [[nodiscard]]
    auto blockList() noexcept -> std::array<AtomicT, kN>&;

    //!
    [[nodiscard]]
    auto blockList() const noexcept -> const std::array<AtomicT, kN>&;

   private:
    std::array<AtomicT, kN> block_list_;
  };


  //! Return the reference of a block
  [[nodiscard]]
  auto getBlockRef(const std::size_t pos) noexcept -> AReference;

  //! Return the reference of a block
  [[nodiscard]]
  auto getBlockRef(const std::size_t pos) const noexcept -> AConstReference;

  //!
  template <typename Func>
  auto iterate(const std::size_t begin,
               const std::size_t end,
               Func func) const noexcept -> std::size_t
  requires std::invocable<Func, Bitset::ConstT, Bitset::AConstReference, std::size_t&>;

  //! Make a bit mask
  static auto makeMask(const std::size_t pos) noexcept -> BitT;

  //! Make a bit mask
  static auto makeMask(const std::size_t begin, const std::size_t end) noexcept -> BitT;


  std::pmr::vector<Chunk> chunk_list_;
  std::size_t size_;
};

} // namespace zisc

#include "bitset-inl.hpp"

#endif // ZISC_BITSET_HPP
