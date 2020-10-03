/*!
  \file fnv_1a_hash_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FNV_1A_HASH_ENGINE_HPP
#define ZISC_FNV_1A_HASH_ENGINE_HPP

// Standard C++ library
#include <cstdint>
#include <cstddef>
// Zisc
#include "hash_engine.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam ValueT No description.
  */
template <HashValue ValueT>
class Fnv1aHashEngine : public HashEngine<Fnv1aHashEngine<ValueT>, ValueT>
{
 public:
  using BaseEngine = HashEngine<Fnv1aHashEngine, ValueT>;
  using ValueType = typename BaseEngine::ValueType;


  //! Implementation of the hash function
  template <HashKeyElement Int8>
  static constexpr ValueType hashValue(const Int8* seed,
                                       const std::size_t n) noexcept;
 private:

  //!
  static constexpr ValueType prime() noexcept;

  //!
  static constexpr ValueType offset() noexcept;
};

// Type aliases

using Fnv1aHash32 = Fnv1aHashEngine<uint32b>;
using Fnv1aHash64 = Fnv1aHashEngine<uint64b>;

} // namespace zisc

#include "fnv_1a_hash_engine-inl.hpp"

#endif // ZISC_FNV_1A_HASH_ENGINE_HPP
