/*!
  \file sip_hash_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SIP_HASH_ENGINE_HPP
#define ZISC_SIP_HASH_ENGINE_HPP

// Standard C++ library
#include <array>
#include <cstdint>
#include <cstddef>
#include <type_traits>
// Zisc
#include "hash_engine.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam ResultType No description.
  */
template <typename ResultType>
class SipHashEngine : public HashEngine<SipHashEngine<ResultType>, ResultType>
{
 private:
  //!
  static constexpr int bShift() noexcept;

  //!
  static constexpr std::size_t cRounds() noexcept;

  //!
  static constexpr std::size_t dRounds() noexcept;

  //!
  static constexpr std::array<ResultType, 2> kList() noexcept;

  template <typename, typename> friend class HashEngine;
  //! Implementation of the hash function
  template <typename Int8>
  static constexpr ResultType hashValue(const Int8* inputs,
                                        const std::size_t n) noexcept;

  //!
  static constexpr std::array<ResultType, 4> initVList() noexcept;

  //!
  static constexpr ResultType rotate(const ResultType x,
                                     const ResultType b) noexcept;

  //!
  static constexpr std::array<ResultType, 6> rotationBitList() noexcept;

  //!
  static constexpr std::array<ResultType, 4> round(
      std::array<ResultType, 4> v_list) noexcept;

  //!
  template <typename Int8>
  static constexpr ResultType toUIntLe(const Int8* inputs) noexcept;

  //!
  static constexpr std::array<ResultType, 4> vList() noexcept;
};

using SipHash32 = SipHashEngine<uint32b>;
using SipHash64 = SipHashEngine<uint64b>;

} // namespace zisc

#include "sip_hash_engine-inl.hpp"

#endif // ZISC_SIP_HASH_ENGINE_HPP
