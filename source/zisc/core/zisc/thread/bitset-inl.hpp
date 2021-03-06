/*!
  \file bitset-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BITSET_INL_HPP
#define ZISC_BITSET_INL_HPP

#include "bitset.hpp"
// Standard C++ library
#include <cstddef>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
// Zisc
#include "atomic.hpp"
#include "zisc/algorithm.hpp"
#include "zisc/bit.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
inline
Bitset::Bitset(pmr::memory_resource* mem_resource) noexcept :
    Bitset(defaultSize(), mem_resource)
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
  block_list_ = other.block_list_;
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
  const std::size_t s = std::numeric_limits<ValueType>::digits;
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
  auto func = [](ConstType mask, ConstReference block, Reference result) noexcept
  {
    ConstType b = atomic_load(std::addressof(block));
    const ValueType v = b & mask;
    result += cast<ValueType>(popcount(v));
    return 1;
  };
  const std::size_t result = iterate(begin, end, func);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t Bitset::defaultSize() noexcept
{
  const std::size_t s = 128;
  return s;
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
  auto func = [](ConstType mask, ConstReference block, Reference result) noexcept
  {
    ConstType b = atomic_load(std::addressof(block));
    const bool flag = (b & mask) == mask;
    result = flag ? 1 : 0;
    return result;
  };
  const bool result = (begin < end) && iterate(begin, end, func);
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
  auto func = [](ConstType mask, ConstReference block, Reference result) noexcept
  {
    ConstType b = atomic_load(std::addressof(block));
    const bool flag = (b & mask) == 0;
    result = flag ? 1 : 0;
    return result;
  };
  const bool result = (end <= begin) || iterate(begin, end, func);
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
  auto func = [value](ConstType mask, ConstReference block, Reference) noexcept
  {
    Reference b = *const_cast<Pointer>(std::addressof(block));
    if (value) {
      ConstType v = mask;
      b = b | v;
    }
    else {
      ConstType v = ~mask;
      b = b & v;
    }
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
  ConstReference block = getBlock(pos);
  ConstType mask = makeMask(pos);
  ConstType b = atomic_load(std::addressof(block));
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
  Reference block = getBlock(pos);
  Pointer blockp = std::addressof(block);
  ConstType mask = makeMask(pos);
  ConstType v = value ? mask : ~mask;
  ConstType old = value ? atomic_fetch_or(blockp, v) : atomic_fetch_and(blockp, v);
  const bool result = (old & mask) != 0;
  return result;
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::getBlock(const std::size_t pos) noexcept -> Reference
{
  constexpr std::size_t bits = blockBitSize();
  const std::size_t i = pos / bits;
  return block_list_[i];
}

/*!
  \details No detailed description

  \param [in] pos No description.
  \return No description
  */
inline
auto Bitset::getBlock(const std::size_t pos) const noexcept -> ConstReference
{
  constexpr std::size_t bits = blockBitSize();
  const std::size_t i = pos / bits;
  return block_list_[i];
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
                     Func func) const noexcept -> ValueType
{
  constexpr std::size_t bits = blockBitSize();
  constexpr std::size_t bitmask = ~(bits - 1);
  ValueType result = 0;
  for (std::size_t pos = begin & bitmask; pos < end; pos = pos + bits) {
    const std::size_t b = (max)(pos, begin);
    const std::size_t e = (min)(pos + bits, end);
    ConstType mask = makeMask(b, e);
    ConstReference block = getBlock(pos);
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
auto Bitset::makeMask(const std::size_t pos) noexcept -> ValueType
{
  constexpr std::size_t bits = blockBitSize();
  constexpr std::size_t bitmask = bits - 1;
  const std::size_t index = pos & bitmask;
  ConstType mask = ValueType{0b1} << index;
  return mask;
}

/*!
  \details No detailed description

  \param [in] begin No description.
  \param [in] end No description.
  \return No description
  */
inline
auto Bitset::makeMask(const std::size_t begin, const std::size_t end) noexcept
    -> ValueType
{
  constexpr std::size_t bits = blockBitSize();
  constexpr std::size_t bitmask = bits - 1;
  ValueType mask = (std::numeric_limits<ValueType>::max)();
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
