/*!
  \file xorshift_engine.hpp
  \author Sho Ikeda
  
  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XORSHIFT_ENGINE_HPP
#define ZISC_XORSHIFT_ENGINE_HPP

// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
 \brief Xorshift random number generator
 \details
 No detailed.
 */
template <typename Type>
class XorshiftEngine :
    public PseudoRandomNumberEngine<XorshiftEngine<Type>, Type, Type>
{
 public:
  using SeedType = Type;
  using ResultType = Type;


  //! Initialize generator using default random seed
  XorshiftEngine() noexcept;

  //! Initialize generator
  XorshiftEngine(const SeedType seed) noexcept;


  //! Generate a random number
  ResultType generate() noexcept;

  //! Set random seed
  void setSeed(const SeedType seed) noexcept;

 private:
  static constexpr uint N = 16 / sizeof(SeedType);


  SeedType seed_[N];
};

using Xorshift32 = XorshiftEngine<uint32>;
using Xorshift128Plus = XorshiftEngine<uint64>;

} // namespace zisc

#include "xorshift_engine-inl.hpp"

#endif // ZISC_XORSHIFT_ENGINE_HPP
