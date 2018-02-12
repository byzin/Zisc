/*!
  \file dimension.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_DIMENSION_HPP
#define ZISC_DIMENSION_HPP

// Standard C++ library
#include <initializer_list>
// Zisc
#include "arith_array.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN>
class Dimension
{
 public:
  using ArrayType = ArithArray<Arith, kN>;


  //! Initialize elements with 0
  constexpr Dimension() noexcept;

  //! Initialize elements with values
  constexpr Dimension(std::initializer_list<Arith> init_list) noexcept;

  //! Initialize elements
  constexpr Dimension(const ArrayType& other) noexcept;

  //! Initialize elements
  constexpr Dimension(ArrayType&& other) noexcept;


  //! Return the reference by index.
  constexpr Arith& operator[](const uint index) noexcept;

  //! Return the reference by index.
  constexpr const Arith& operator[](const uint index) const noexcept;

  //! Return the array body
  constexpr ArrayType& data() noexcept;

  //! Return the array body
  constexpr const ArrayType& data() const noexcept;

  //! Return the reference by index.
  constexpr Arith& get(const uint index) noexcept;

  //! Return the reference by index.
  constexpr const Arith& get(const uint index) const noexcept;

  //! Return the number of elements
  static constexpr uint size() noexcept;

  //! Set value
  constexpr void set(const uint index, const Arith value) noexcept;

  //! Set values
  constexpr void set(std::initializer_list<Arith> init_list) noexcept;

 private:
  ArrayType data_;
};

} // namespace zisc

#include "dimension-inl.hpp"

#endif // ZISC_DIMENSION_HPP
