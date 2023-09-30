/*!
  \file ring_buffer-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_RING_BUFFER_INL_HPP
#define ZISC_RING_BUFFER_INL_HPP

#include "ring_buffer.hpp"
// Standard C++ library
#include <bit>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
constexpr std::size_t RingBuffer<RingBufferClass>::capacityMax() noexcept
{
  const auto c = static_cast<std::size_t>(indexMask());
  return c;
}

/*!
  \details No detailed description
  */
template <typename RingBufferClass> inline
void RingBuffer<RingBufferClass>::clear() noexcept
{
  RingBufferReference buffer = ref();
  buffer.clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
constexpr uint64b RingBuffer<RingBufferClass>::overflowIndex() noexcept
{
  constexpr uint64b index = invalidIndex() - 1;
  return index;
}

/*!
  \details No detailed description

  \param [in] nonempty No description.
  \return No description
  */
template <typename RingBufferClass> inline
uint64b RingBuffer<RingBufferClass>::dequeue(const bool nonempty) noexcept
{
  RingBufferReference buffer = ref();
  return buffer.dequeue(nonempty);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
std::size_t RingBuffer<RingBufferClass>::distance() const noexcept
{
  ConstRingBufferReference buffer = ref();
  return buffer.distance();
}

/*!
  \details No detailed description

  \param [in] index No description.
  \param [in] nonempty No description.
  \return No description
  */
template <typename RingBufferClass> inline
bool RingBuffer<RingBufferClass>::enqueue(const uint64b index, const bool nonempty) noexcept
{
  RingBufferReference buffer = ref();
  return buffer.enqueue(index, nonempty);
}

/*!
  \details No detailed description
  */
template <typename RingBufferClass> inline
void RingBuffer<RingBufferClass>::full() noexcept
{
  RingBufferReference buffer = ref();
  buffer.full();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
constexpr uint64b RingBuffer<RingBufferClass>::indexMask() noexcept
{
  constexpr uint64b mask = (std::numeric_limits<uint64b>::max)() >> 1;
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
constexpr uint64b RingBuffer<RingBufferClass>::invalidIndex() noexcept
{
  constexpr uint64b invalid = (std::numeric_limits<uint64b>::max)();
  return invalid;
}

/*!
  \details No detailed description

  \tparam kDataSize No description.
  \param [in] index No description.
  \param [in] n No description.
  \return No description
  */
template <typename RingBufferClass> template <std::size_t kDataSize> inline
uint64b RingBuffer<RingBufferClass>::permuteIndex(const uint64b index, const uint64b n) noexcept
{
  static_assert(std::has_single_bit(kDataSize), "The data size isn't power of 2.");
  ZISC_ASSERT(std::has_single_bit(n), "The n isn't power of 2.");
  const uint64b o = calcOrder(n);
  constexpr uint64b cache_line_size = kCacheLineSize;
  constexpr auto data_size = static_cast<uint64b>(kDataSize);
  constexpr uint64b shift = (data_size < cache_line_size)
      ? std::bit_width(cache_line_size) - std::bit_width(data_size)
      : 0;

  uint64b i = index;
  if (shift < o) {
    const uint64b upper = cast<uint64b>(index << shift);
    const uint64b lower = cast<uint64b>((index & (n - 1)) >> (o - shift));
    i = cast<uint64b>(upper | lower);
  }
  i = i & (n - 1);
  return i;
}

/*!
  \details No detailed description

  \param [in] s \a s must be a power of 2.
  */
template <typename RingBufferClass> inline
void RingBuffer<RingBufferClass>::setSize(const std::size_t s) noexcept
{
  ZISC_ASSERT((s == 0) || std::has_single_bit(s), "The size isn't 2^n. size = ", s);
  ZISC_ASSERT(s < capacityMax(), "The size exceeds the capacity max. size = ", s);
  RingBufferReference buffer = ref();
  buffer.setSize(s);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
std::size_t RingBuffer<RingBufferClass>::size() const noexcept
{
  ConstRingBufferReference buffer = ref();
  const std::size_t s = buffer.size();
  ZISC_ASSERT((s == 0) || std::has_single_bit(s), "The size isn't 2^n. size = ", s);
  return s;
}

/*!
  \details No detailed description
  */
template <typename RingBufferClass> inline
RingBuffer<RingBufferClass>::RingBuffer() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename RingBufferClass> inline
RingBuffer<RingBufferClass>::RingBuffer([[maybe_unused]] const RingBuffer& other) noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename RingBufferClass> inline
auto RingBuffer<RingBufferClass>::operator =([[maybe_unused]] const RingBuffer& other) noexcept -> RingBuffer&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
uint64b RingBuffer<RingBufferClass>::calcOrder(const uint64b s) noexcept
{
  const uint64b o = cast<uint64b>(std::bit_width(s));
  return (0 < o) ? o - 1 : 0;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
auto RingBuffer<RingBufferClass>::ref() noexcept -> RingBufferReference
{
  auto* buffer = static_cast<RingBufferPtr>(this);
  return *buffer;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename RingBufferClass> inline
auto RingBuffer<RingBufferClass>::ref() const noexcept -> ConstRingBufferReference
{
  auto* buffer = static_cast<ConstRingBufferPtr>(this);
  return *buffer;
}

} // namespace zisc

#endif // ZISC_RING_BUFFER_INL_HPP
