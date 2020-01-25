/*!
  \file padded_value-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PADDED_VALUE_INL_HPP
#define ZISC_PADDED_VALUE_INL_HPP

#include "padded_value.hpp"
// Standard C++ library
#include <cstddef>
#include <type_traits>
#include <utility>

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  */
template <typename Type>
struct PaddedValueImpl<Type, 0>
{
  using ConstType = std::add_const_t<Type>;
  using Reference = std::add_lvalue_reference_t<Type>;
  using RReference = std::add_rvalue_reference_t<Type>;
  using ConstReference = std::add_lvalue_reference_t<ConstType>;


  //! Create a value
  PaddedValueImpl() noexcept {}

  //! Copy a data
  PaddedValueImpl(ConstReference other) noexcept : value_{other} {}

  //! Move a data
  PaddedValueImpl(RReference other) noexcept : value_{std::move(other)} {}


  //! Copy a data
  PaddedValueImpl& operator=(ConstReference other) noexcept
  {
    value_ = other;
    return *this;
  }

  //! Move a data
  PaddedValueImpl& operator=(RReference other) noexcept
  {
    value_ = std::move(other);
    return *this;
  }


  Type value_;
};

/*!
  \details No detailed description
  */
template <typename Type, std::size_t kSize> inline
PaddedValue<Type, kSize>::PaddedValue() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type, std::size_t kSize> inline
PaddedValue<Type, kSize>::PaddedValue(RReference other) noexcept :
    impl_{std::move(other)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type, std::size_t kSize> inline
PaddedValue<Type, kSize>::PaddedValue(PaddedValue&& other) noexcept :
    impl_{std::move(other.impl_.value_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename Type, std::size_t kSize> inline
auto PaddedValue<Type, kSize>::operator=(RReference other) noexcept
    -> PaddedValue&
{
  impl_ = std::move(other);
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename Type, std::size_t kSize> inline
auto PaddedValue<Type, kSize>::operator=(PaddedValue&& other) noexcept
    -> PaddedValue&
{
  impl_ = std::move(other.impl_.value_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, std::size_t kSize> inline
auto PaddedValue<Type, kSize>::get() noexcept -> Reference
{
  return impl_.value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, std::size_t kSize> inline
auto PaddedValue<Type, kSize>::get() const noexcept -> ConstReference
{
  return impl_.value_;
}

} // namespace zisc

#endif // ZISC_PADDED_VALUE_INL_HPP
