/*!
  \file sip_hash_engine-inl.hppp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SIP_HASH_ENGINE_INL_HPP
#define ZISC_SIP_HASH_ENGINE_INL_HPP

#include "sip_hash_engine.hpp"
// Standard C++ library
#include <array>
#include <cstdint>
#include <cstddef>
#include <type_traits>
// Zisc
#include "string.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

namespace inner {

template <typename ResultType> struct SipHashEngineImpl;

template <>
struct SipHashEngineImpl<uint32>
{
  static constexpr std::size_t c_rounds = 2;
  static constexpr std::size_t d_rounds = 4;
  static constexpr std::array<uint32, 4> v_list{{0,
                                                 0,
                                                 0x6c796765u,
                                                 0x74656462u}};
  static constexpr std::array<uint32, 6> rotation_bit_list{{5, 16, 8, 7, 13, 16}};
  static constexpr int b_shift = 24;
};

template <>
struct SipHashEngineImpl<uint64>
{
  static constexpr std::size_t c_rounds = 2;
  static constexpr std::size_t d_rounds = 4;
  static constexpr std::array<uint64, 4> v_list{{0x736f6d6570736575ull,
                                                 0x646f72616e646f6dull,
                                                 0x6c7967656e657261ull,
                                                 0x7465646279746573ull}};
  static constexpr std::array<uint64, 6> rotation_bit_list{{13, 32, 16, 21, 17, 32}};
  static constexpr int b_shift = 56;
};

} // namespace inner

/*!
  */
template <typename ResultType> inline
constexpr std::array<ResultType, 2> SipHashEngine<ResultType>::kList() noexcept
{
  std::array<ResultType, 2> k_list{{0, 0}};
  for (std::size_t i = 0; i < 2; ++i) {
    constexpr std::size_t n = sizeof(ResultType);
    std::array<uint8, n> k{};
    for (std::size_t byte = 0; byte < n; ++byte)
      k[byte] = cast<uint8>(byte + (i * n));
    k_list[i] = toUIntLe(k.data());
  }
  return k_list;
}

/*!
  */
template <typename ResultType> template <typename Int8> inline
constexpr ResultType SipHashEngine<ResultType>::hashValue(
    const Int8* inputs,
    const std::size_t n) noexcept
{
  static_assert(sizeof(Int8) == 1, "Int8 isn't 8bit integer.");
  using Impl = inner::SipHashEngineImpl<ResultType>;
  constexpr std::size_t hash_size = sizeof(ResultType);

  auto v_list = initVList();

  const std::size_t left = n & (hash_size - 1);
  const std::size_t end = n - left;
  for (std::size_t i = 0; i < end; i = i + hash_size) {
    const auto m = toUIntLe(inputs + i);
    v_list[3] ^= m;
    for (std::size_t j = 0; j < Impl::c_rounds; ++j)
      v_list = round(v_list);
    v_list[0] ^= m;
  }

  ResultType b = cast<ResultType>(n) << Impl::b_shift;
  for (std::size_t i = left; 0 < i; --i)
    b |= cast<ResultType>(inputs[n - (1 + (left - i))]) << (8 * (i - 1));

  v_list[3] ^= b;
  for (std::size_t i = 0; i < Impl::c_rounds; ++i)
    v_list = round(v_list);
  v_list[0] ^= b;
  v_list[2] ^= cast<ResultType>(0xff);

  for (std::size_t i = 0; i < Impl::d_rounds; ++i)
    v_list = round(v_list);
  b = (hash_size == 4)
      ? v_list[1] ^ v_list[3]
      : v_list[0] ^ v_list[1] ^ v_list[2] ^ v_list[3] ;

  return b;
}

/*!
  */
template <typename ResultType> inline
constexpr std::array<ResultType, 4> SipHashEngine<ResultType>::initVList() noexcept
{
  auto v_list = inner::SipHashEngineImpl<ResultType>::v_list;
  {
    constexpr auto k_list = kList();
    for (std::size_t i = 0; i < v_list.size(); ++i)
      v_list[i] ^= k_list[isOdd(i) ? 1 : 0];
  }
  return v_list;
}

/*!
  */
template <typename ResultType> inline
constexpr ResultType SipHashEngine<ResultType>::rotate(
    const ResultType x,
    const ResultType b) noexcept
{
  constexpr ResultType bit = cast<ResultType>(8 * sizeof(ResultType));
  const ResultType result = (x << b) | (x >> (bit - b));
  return result;
}

/*!
  */
template <typename ResultType> inline
constexpr std::array<ResultType, 4> SipHashEngine<ResultType>::round(
    std::array<ResultType, 4> v_list) noexcept
{
  constexpr auto rotation_bits = inner::SipHashEngineImpl<ResultType>::rotation_bit_list;
  v_list[0] += v_list[1];
  v_list[1] = rotate(v_list[1], rotation_bits[0]);
  v_list[1] ^= v_list[0];
  v_list[0] = rotate(v_list[0], rotation_bits[1]);
  v_list[2] += v_list[3];
  v_list[3] = rotate(v_list[3], rotation_bits[2]);
  v_list[3] ^= v_list[2];
  v_list[0] += v_list[3];
  v_list[3] = rotate(v_list[3], rotation_bits[3]);
  v_list[3] ^= v_list[0];
  v_list[2] += v_list[1];
  v_list[1] = rotate(v_list[1], rotation_bits[4]);
  v_list[1] ^= v_list[2];
  v_list[2] = rotate(v_list[2], rotation_bits[5]);
  return v_list;
}

/*!
  */
template <typename ResultType> template <typename Int8> inline
constexpr ResultType SipHashEngine<ResultType>::toUIntLe(
    const Int8* inputs) noexcept
{
  static_assert(sizeof(Int8) == 1, "Int8 isn't 8bit integer.");
  ResultType result = 0;
  for (std::size_t i = 0; i < sizeof(ResultType); ++i) {
    const ResultType input = cast<ResultType>(inputs[i]);
    result = result | (input << (8 * i));
  }
  return result;
}

} // namespace zisc

#endif // ZISC_SIP_HASH_ENGINE_INL_HPP
