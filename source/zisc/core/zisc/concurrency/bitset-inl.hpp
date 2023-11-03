/*!
  \file bitset-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BITSET_INL_HPP
#define ZISC_BITSET_INL_HPP

#include "bitset.hpp"
// Standard C++ library
#include <bit>
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
Bitset::Bitset(pmr::memory_resource* mem_resource) noexcept :
    Bitset(1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] s No description.
  \param [in,out] mem_resource No description.
  */
inline
Bitset::Bitset(const std::size_t s, pmr::memory_resource* mem_resource) noexcept :
    chunk_list_{decltype(chunk_list_)::allocator_type{mem_resource}},
    size_{0}
{
  constexpr std::size_t num_of_attempts = 16;
  for (std::size_t i = 0; i < num_of_attempts; ++i) {
    try {
      setSize(s);
      break;
    }
    catch ([[maybe_unused]] const std::exception& error) {
    }
  }
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Bitset::Bitset(Bitset&& other) noexcept :
    chunk_list_{std::move(other.chunk_list_)},
    size_{other.size_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
auto Bitset::operator=(Bitset&& other) noexcept -> Bitset&
{
  chunk_list_ = std::move(other.chunk_list_);
  size_ = other.size_;
  return *this;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::operator[](const std::size_t pos) const noexcept -> bool
{
  const bool result = test(pos);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto Bitset::blockBitSize() noexcept -> std::size_t
{
  const std::size_t s = 8 * sizeof(AtomicT);
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto Bitset::chunkBitSize() noexcept -> std::size_t
{
  const std::size_t s = 8 * sizeof(Chunk);
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Bitset::count() const noexcept -> std::size_t
{
  const std::size_t c = count(0, size());
  return c;
}

/*!
  \details No detailed description

  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
inline
auto Bitset::count(const std::size_t begin, const std::size_t end) const noexcept -> std::size_t
{
  auto func = [](ConstT mask, AConstReference block, std::size_t& result) noexcept
  {
    ConstT b = block.load(std::memory_order::acquire);
    const BitT v = b & mask;
    result += cast<std::size_t>(std::popcount(v));
    return 1;
  };
  const std::size_t result = iterate(begin, end, func);
  return result;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::getBlockBits(const std::size_t pos) const noexcept -> BitT
{
  AConstReference block = getBlockRef(pos);
  return block.load(std::memory_order::acquire);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Bitset::isAll() const noexcept -> bool
{
  const bool result = isAll(0, size());
  return result;
}

/*!
  \details No detailed description

  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
inline
auto Bitset::isAll(const std::size_t begin, const std::size_t end) const noexcept -> bool
{
  auto func = [](ConstT mask, AConstReference block, std::size_t& result) noexcept
  {
    ConstT b = block.load(std::memory_order::acquire);
    const bool flag = (b & mask) == mask;
    result = flag ? 1 : 0;
    return flag;
  };
  const bool result = (begin < end) && (iterate(begin, end, func) == 1);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Bitset::isAny() const noexcept -> bool
{
  const bool result = isAny(0, size());
  return result;
}

/*!
  \details No detailed description

  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
inline
auto Bitset::isAny(const std::size_t begin, const std::size_t end) const noexcept -> bool
{
  const bool result = !isNone(begin, end);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Bitset::isNone() const noexcept -> bool
{
  const bool result = isNone(0, size());
  return result;
}

/*!
  \details No detailed description

  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
inline
auto Bitset::isNone(const std::size_t begin, const std::size_t end) const noexcept -> bool
{
  auto func = [](ConstT mask, AConstReference block, std::size_t& result) noexcept
  {
    ConstT b = block.load(std::memory_order::acquire);
    const bool flag = (mask & b) == 0;
    result = flag ? 1 : 0;
    return flag;
  };
  const bool result = (end <= begin) || (iterate(begin, end, func) == 1);
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
inline
void Bitset::reset(const bool value) noexcept
{
  reset(0, size(), value);
}

/*!
  \details No detailed description

  \param [in] begin No description.
  \param [in] end No description.
  \param [in] value No description.
  */
inline
void Bitset::reset(const std::size_t begin, const std::size_t end, const bool value) noexcept
{
  auto func = [value](ConstT mask, AConstReference block, std::size_t&) noexcept
  {
    AReference mutable_block = *const_cast<APointer>(std::addressof(block));
    BitT b = mutable_block.load(std::memory_order::acquire);
    if (value) {
      ConstT v = mask;
      b = b | v;
    }
    else {
      ConstT v = ~mask;
      b = b & v;
    }
    mutable_block.store(b, std::memory_order::release);
    return 1;
  };
  iterate(begin, end, func);
}

/*!
  \details No detailed description

  \param [in] s No description.
  */
inline
void Bitset::setSize(const std::size_t s)
{
  constexpr std::size_t bits = chunkBitSize();
  const std::size_t n = (s + (bits - 1)) / bits;
  chunk_list_.resize(n);
  size_ = s;
  reset();
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Bitset::size() const noexcept -> std::size_t
{
  return size_;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::test(const std::size_t pos) const noexcept -> bool
{
  AConstReference block = getBlockRef(pos);
  ConstT mask = makeMask(pos);
  ConstT b = block.load(std::memory_order::acquire);
  const bool result = (b & mask) != 0;
  return result;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \param [in] value No description.
  \return No description
  */
inline
auto Bitset::testAndSet(const std::size_t pos, const bool value) noexcept -> bool
{
  AReference block = getBlockRef(pos);
  ConstT mask = makeMask(pos);
  ConstT v = value ? mask : ~mask;
  ConstT old = value ? block.fetch_or(v, std::memory_order::acq_rel)
                     : block.fetch_and(v, std::memory_order::acq_rel);
  const bool result = (old & mask) != 0;
  return result;
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Bitset::Chunk::Chunk(Chunk&& other) noexcept
{
  set(other);
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
auto Bitset::Chunk::operator=(Chunk&& other) noexcept -> Chunk&
{
  set(other);
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
void Bitset::Chunk::set(const Chunk& other) noexcept
{
  for (std::size_t i = 0; i < block_list_.size(); ++i) {
    const BitT v = other.block_list_[i].load(std::memory_order::acquire);
    block_list_[i].store(v, std::memory_order::release);
  }
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::getBlockRef(const std::size_t pos) noexcept -> AReference
{
  const std::size_t chunk_index = pos / chunkBitSize();
  Chunk& chunk = chunk_list_[chunk_index];
  const std::size_t block_index = (pos % chunkBitSize()) / blockBitSize();
  return chunk.block_list_[block_index];
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::getBlockRef(const std::size_t pos) const noexcept -> AConstReference
{
  const std::size_t chunk_index = pos / chunkBitSize();
  const Chunk& chunk = chunk_list_[chunk_index];
  const std::size_t block_index = (pos % chunkBitSize()) / blockBitSize();
  return chunk.block_list_[block_index];
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] begin No description.
  \param [in] end description.
  \param [in] func No description.
  \return No description
  */
template <typename Func> inline
auto Bitset::iterate(const std::size_t begin,
                     const std::size_t end,
                     Func func) const noexcept -> std::size_t
    requires std::invocable<Func, Bitset::ConstT, Bitset::AConstReference, std::size_t&>
{
  constexpr std::size_t bits = blockBitSize();
  constexpr std::size_t bitmask = ~(bits - 1);
  std::size_t result = 0;
  for (std::size_t pos = begin & bitmask; pos < end; pos = pos + bits) {
    const std::size_t b = (max)(pos, begin);
    const std::size_t e = (min)(pos + bits, end);
    ConstT mask = makeMask(b, e);
    AConstReference block = getBlockRef(pos);
    if (!func(mask, block, result))
      break;
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::makeMask(const std::size_t pos) noexcept -> BitT
{
  constexpr std::size_t bits = blockBitSize();
  constexpr std::size_t bitmask = bits - 1;
  const std::size_t index = pos & bitmask;
  ConstT mask = BitT{0b1} << index;
  return mask;
}

/*!
  \details No detailed description

  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
inline
auto Bitset::makeMask(const std::size_t begin, const std::size_t end) noexcept -> BitT
{
  constexpr std::size_t bits = blockBitSize();
  constexpr std::size_t bitmask = bits - 1;
  BitT mask = (std::numeric_limits<BitT>::max)();
  {
    const std::size_t b = begin & bitmask;
    mask = mask >> b;
    mask = mask << b;
  }
  {
    const std::size_t e = bits - (((end - 1) & bitmask) + 1);
    mask = mask << e;
    mask = mask >> e;
  }
  return mask;
}

} // namespace zisc

#endif // ZISC_BITSET_INL_HPP
