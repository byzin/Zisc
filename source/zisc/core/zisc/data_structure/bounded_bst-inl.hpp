/*!
  \file bounded_bst-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BOUNDED_BST_INL_HPP
#define ZISC_BOUNDED_BST_INL_HPP

#include "bounded_bst.hpp"
// Standard C++ library
#include <cstddef>
#include <tuple>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <typename BstClass> template <ConvertibleTo<double> Type> inline
auto BoundedBst<BstClass>::add(const Type& value) -> std::tuple<bool, size_type>
{
  auto& bst = ref();
  auto result = bst.add(value);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename BstClass> inline
auto BoundedBst<BstClass>::capacity() const noexcept -> size_type
{
  auto& bst = ref();
  const size_type cap = bst.capacity();
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename BstClass> inline
constexpr auto BoundedBst<BstClass>::capacityMax() noexcept -> size_type
{
  const size_type cap = BstT::capacityMax();
  return cap;
}

/*!
  \details No detailed description
  */
template <typename BstClass> inline
void BoundedBst<BstClass>::clear() noexcept
{
  auto& bst = ref();
  bst.clear();
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <typename BstClass> template <ConvertibleTo<double> Type> inline
bool BoundedBst<BstClass>::contain(const Type& value) const noexcept
{
  auto& bst = ref();
  const bool result = bst.contain(value);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename BstClass> inline
double BoundedBst<BstClass>::findMinKey() const noexcept
{
  auto& bst = ref();
  const double k = bst.findMinKey();
  return k;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <typename BstClass> template <ConvertibleTo<double> Type> inline
bool BoundedBst<BstClass>::remove(const Type& value)
{
  auto& bst = ref();
  const bool result = bst.remove(value);
  return result;
}

/*!
  \details No detailed description
  */
template <typename BstClass> inline
void BoundedBst<BstClass>::setCapacity(const size_type cap) noexcept
{
  auto& bst = ref();
  bst.setCapacity(cap);
}

/*!
  \details No detailed description
  */
template <typename BstClass> inline
BoundedBst<BstClass>::BoundedBst() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename BstClass> inline
BoundedBst<BstClass>::BoundedBst([[maybe_unused]] const BoundedBst& other) noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename BstClass> inline
auto BoundedBst<BstClass>::operator=([[maybe_unused]] const BoundedBst& other) noexcept -> BoundedBst&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename BstClass> inline
auto BoundedBst<BstClass>::ref() noexcept -> BstReference
{
  auto q = cast<BstPtr>(this);
  return *q;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename BstClass> inline
auto BoundedBst<BstClass>::ref() const noexcept -> ConstBstReference
{
  auto q = cast<ConstBstPtr>(this);
  return *q;
}

} // namespace zisc

#endif // ZISC_BOUNDED_BST_INL_HPP
