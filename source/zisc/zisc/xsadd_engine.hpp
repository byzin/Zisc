/*!
  \file xsadd_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XSADD_ENGINE_HPP
#define ZISC_XSADD_ENGINE_HPP

// Standard C++ library
#include <cstddef>
#include <cstdint>
#include <vector>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Xorshift-add random number generator
  \details
  No detailed.
  */
class XsaddEngine : public PseudoRandomNumberEngine<XsaddEngine, uint32, uint32>
{
 public:
  using SeedType = uint32;
  using ResultType = uint32;


  //! Initialize generator using defualt random seed
  XsaddEngine() noexcept;

  //! Initialize generator
  XsaddEngine(const uint32 seed) noexcept;


  //! Generate a random number
  uint32 generate() noexcept;

  //! Set random seed
  void setSeed(const uint32 seed) noexcept;

  //! Set random seed
  void setSeed(const std::vector<uint32>& seed_array) noexcept;

 private:
  //! Initialize
  uint32 initializeValue1(const uint32 x) const noexcept;

  //! Initialize
  uint32 initializeValue2(const uint32 x) const noexcept;

  //! Change internal state
  void nextState() noexcept;

  //! Certificate period
  void periodCertification() noexcept;


  static constexpr uint loop_ = 8;


  uint32 state_[4];
};

} // namespace zisc

#include "xsadd_engine-inl.hpp"

#endif // ZISC_XSADD_ENGINE_HPP
