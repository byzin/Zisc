/*!
  \file bitset.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BITSET_HPP
#define ZISC_BITSET_HPP

// Standard C++ library
#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>
// Zisc
#include "atomic.hpp"
#include "non_copyable.hpp"
#include "zisc_config.hpp"
#include "memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class Bitset : private NonCopyable<Bitset>
{
 public:
  // Types
  using ValueType = uint64b;
  using ConstType = std::add_const_t<ValueType>;
  using Reference = std::add_lvalue_reference_t<ValueType>;
  using ConstReference = std::add_lvalue_reference_t<ConstType>;
  using Pointer = std::add_pointer_t<ValueType>;
  using ConstPointer = std::add_pointer_t<ConstType>;


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

  //! Return the default size
  static constexpr std::size_t defaultSize() noexcept;

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

  //! Set a bit to the given value
  void set(const std::size_t pos, const bool value) noexcept;

  //! Set the number of bits
  void setSize(const std::size_t s) noexcept;

  //! Return the number of bits
  std::size_t size() const noexcept;

  //! Access the bit at position pos
  bool test(const std::size_t pos) const noexcept;

 private:
  //! Return the reference of a block
  Reference getBlock(const std::size_t pos) noexcept;

  //! Return the reference of a block
  ConstReference getBlock(const std::size_t pos) const noexcept;

  //!
  template <typename Func>
  ValueType iterate(const std::size_t begin,
                    const std::size_t end,
                    Func func) const noexcept;

  //! Make a bit mask
  static ValueType makeMask(const std::size_t pos) noexcept;

  //! Make a bit mask
  static ValueType makeMask(const std::size_t begin, const std::size_t end) noexcept;


  pmr::vector<ValueType> block_list_;
  std::size_t size_;
};

} // namespace zisc

#include "bitset-inl.hpp"

#endif // ZISC_BITSET_HPP
