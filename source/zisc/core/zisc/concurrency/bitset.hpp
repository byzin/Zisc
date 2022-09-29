/*!
  \file bitset.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BITSET_HPP
#define ZISC_BITSET_HPP

// Standard C++ library
#include <atomic>
#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

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
  Bitset(pmr::memory_resource* mem_resource) noexcept;

  //! Create a bitset
  Bitset(const std::size_t s, pmr::memory_resource* memory_resource) noexcept;

  //! Move a data
  Bitset(Bitset&& other) noexcept;


  //! Move a data
  Bitset& operator=(Bitset&& other) noexcept;

  //! Access the bit at position pos
  bool operator[](const std::size_t pos) const noexcept;


  //! Return the bit size of a block
  static constexpr std::size_t blockBitSize() noexcept;

  //! Return the number of bits set to true
  std::size_t count() const noexcept;

  //! Return the number of bits set to true
  std::size_t count(const std::size_t begin, const std::size_t end) const noexcept;

  //! Return the block at the pos
  BitT getBlock(const std::size_t pos) const noexcept;

  //! Check if all bits are set to true
  bool isAll() const noexcept;

  //! Check if all bits are set to true
  bool isAll(const std::size_t begin, const std::size_t end) const noexcept;

  //! Check if any of the bits are set to true
  bool isAny() const noexcept;

  //! Check if any of the bits are set to true
  bool isAny(const std::size_t begin, const std::size_t end) const noexcept;

  //! Check if none of the bits are set to true
  bool isNone() const noexcept;

  //! Check if none of the bits are set to true
  bool isNone(const std::size_t begin, const std::size_t end) const noexcept;

  //! Set bits to the given value
  void reset(const bool value = false) noexcept;

  //! Set bits to the given value
  void reset(const std::size_t begin, const std::size_t end, const bool value = false) noexcept;

  //! Set the number of bits
  void setSize(const std::size_t s) noexcept;

  //! Return the number of bits
  std::size_t size() const noexcept;

  //! Return the value at the given position
  bool test(const std::size_t pos) const noexcept;

  //! Atomically set value and obtain it's previous value
  bool testAndSet(const std::size_t pos, const bool value) noexcept;

 private:
  // Type aliases
  using AtomicT = std::atomic<BitT>;
  using AConstT = std::add_const_t<AtomicT>;
  using AReference = std::add_lvalue_reference_t<AtomicT>;
  using AConstReference = std::add_lvalue_reference_t<AConstT>;
  using APointer = std::add_pointer_t<AtomicT>;
  using AConstPointer = std::add_pointer_t<AConstT>;

  struct Wrapper
  {
    //! Create a wrapper
    Wrapper() noexcept;

    //! Move a data
    Wrapper(Wrapper&& other) noexcept;

    //! Move a data
    Wrapper& operator=(Wrapper&& other) noexcept;

    AtomicT value_;
  };


  //! Return the reference of a block
  AReference getBlockRef(const std::size_t pos) noexcept;

  //! Return the reference of a block
  AConstReference getBlockRef(const std::size_t pos) const noexcept;

  //!
  template <typename Func>
  std::size_t iterate(const std::size_t begin,
                      const std::size_t end,
                      Func func) const noexcept
  requires std::invocable<Func, Bitset::ConstT, Bitset::AConstReference, std::size_t&>;

  //! Make a bit mask
  static BitT makeMask(const std::size_t pos) noexcept;

  //! Make a bit mask
  static BitT makeMask(const std::size_t begin, const std::size_t end) noexcept;


  pmr::vector<Wrapper> block_list_;
  std::size_t size_;
};

} // namespace zisc

#include "bitset-inl.hpp"

#endif // ZISC_BITSET_HPP
