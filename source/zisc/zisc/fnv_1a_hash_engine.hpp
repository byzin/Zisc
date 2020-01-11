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
#include <array>
#include <cstdint>
#include <cstddef>
#include <type_traits>
// Zisc
#include "hash_engine.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam ResultType No description.
  */
template <typename ResultType>
class Fnv1aHashEngine : public HashEngine<Fnv1aHashEngine<ResultType>, ResultType>
{
 private:
  template <typename, typename> friend class HashEngine;
  //! Implementation of the hash function
  template <typename Int8>
  static constexpr ResultType hashValue(const Int8* seed,
                                        const std::size_t n) noexcept;

  //!
  static constexpr ResultType prime() noexcept;

  //!
  static constexpr ResultType offset() noexcept;
};

using Fnv1aHash32 = Fnv1aHashEngine<uint32b>;
using Fnv1aHash64 = Fnv1aHashEngine<uint64b>;

} // namespace zisc

#include "fnv_1a_hash_engine-inl.hpp"

#endif // ZISC_FNV_1A_HASH_ENGINE_HPP
