/*!
  \file bitset-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
    block_list_{decltype(block_list_)::allocator_type{mem_resource}}
{
  setSize(s);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Bitset::Bitset(Bitset&& other) noexcept :
    block_list_{std::move(other.block_list_)},
    size_{other.size_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
Bitset& Bitset::operator=(Bitset&& other) noexcept
{
  block_list_ = std::move(other.block_list_);
  size_ = other.size_;
  return *this;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
bool Bitset::operator[](const std::size_t pos) const noexcept
{
  const bool result = test(pos);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t Bitset::blockBitSize() noexcept
{
  const std::size_t s = std::numeric_limits<BitT>::digits;
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Bitset::count() const noexcept
{
  const auto c = count(0, size());
  return c;
}

/*!
  \details No detailed description

  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
inline
std::size_t Bitset::count(const std::size_t begin, const std::size_t end) const noexcept
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
auto Bitset::getBlock(const std::size_t pos) const noexcept -> BitT
{
  AConstReference block = getBlockRef(pos);
  return block.load(std::memory_order::acquire);
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool Bitset::isAll() const noexcept
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
bool Bitset::isAll(const std::size_t begin, const std::size_t end) const noexcept
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
bool Bitset::isAny() const noexcept
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
bool Bitset::isAny(const std::size_t begin, const std::size_t end) const noexcept
{
  const bool result = !isNone(begin, end);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool Bitset::isNone() const noexcept
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
bool Bitset::isNone(const std::size_t begin, const std::size_t end) const noexcept
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
void Bitset::setSize(const std::size_t s) noexcept
{
  constexpr std::size_t bits = blockBitSize();
  const std::size_t s64 = (s + (bits - 1)) / bits;
  block_list_.resize(s64);
  size_ = s;
  reset();
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Bitset::size() const noexcept
{
  return size_;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
bool Bitset::test(const std::size_t pos) const noexcept
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
bool Bitset::testAndSet(const std::size_t pos, const bool value) noexcept
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
  */
inline
Bitset::Wrapper::Wrapper() noexcept
{
}

/*!
  \details No detailed description
  */
inline
Bitset::Wrapper::Wrapper(Wrapper&& other) noexcept :
    value_{other.value_.load(std::memory_order::acquire)}
{
}

/*!
  \details No detailed description
  */
inline
auto Bitset::Wrapper::operator=(Wrapper&& other) noexcept -> Wrapper&
{
  const auto v = other.value_.load(std::memory_order::acquire);
  value_.store(v, std::memory_order::release);
  return *this;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::getBlockRef(const std::size_t pos) noexcept -> AReference
{
  constexpr std::size_t bits = blockBitSize();
  const std::size_t i = pos / bits;
  return block_list_[i].value_;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::getBlockRef(const std::size_t pos) const noexcept -> AConstReference
{
  constexpr std::size_t bits = blockBitSize();
  const std::size_t i = pos / bits;
  return block_list_[i].value_;
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
std::size_t Bitset::iterate(const std::size_t begin,
                            const std::size_t end,
                            Func func) const noexcept
    requires std::invocable<Func, ConstT, AConstReference, std::size_t&>
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
    std::size_t b = begin & bitmask;
    mask = mask >> b;
    mask = mask << b;
  }
  {
    std::size_t e = bits - (((end - 1) & bitmask) + 1);
    mask = mask << e;
    mask = mask >> e;
  }
  return mask;
}

} // namespace zisc

#endif // ZISC_BITSET_INL_HPP
