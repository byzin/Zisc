/*!
  \file cumulative_distribution_function-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_INL_HPP
#define ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_INL_HPP

#include "cumulative_distribution_function.hpp"
// Standard C++ library
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>
#include <vector>
#include <utility>
// Zisc
#include "algorithm.hpp"
#include "compensated_summation.hpp"
#include "error.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
CumulativeDistributionFunction<XType, PdfType>::CumulativeDistributionFunction(
    const std::vector<XType>& x_list,
    const std::vector<PdfType>& y_list) noexcept :
        size_{0},
        capacity_{0}
{
  set(x_list, y_list);
}

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
CumulativeDistributionFunction<XType, PdfType>::CumulativeDistributionFunction(
    std::vector<XType>&& x_list,
    std::vector<PdfType>&& y_list) noexcept :
        size_{0},
        capacity_{0}
{
  set(std::move(x_list), std::move(y_list));
}

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
CumulativeDistributionFunction<XType, PdfType>::CumulativeDistributionFunction(
    CumulativeDistributionFunction&& other) noexcept :
  x_list_{std::move(other.x_list_)},
  y_list_{std::move(other.y_list_)},
  size_{other.size_},
  capacity_{other.capacity_}
{
}

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
auto CumulativeDistributionFunction<XType, PdfType>::operator=(
    CumulativeDistributionFunction&& other) noexcept -> CumulativeDistributionFunction&
{
  x_list_ = std::move(other.x_list_);
  y_list_ = std::move(other.y_list_);
  size_ = other.size_;
  capacity_ = other.capacity_;
}

/*!
  */
template <typename XType, typename PdfType> inline
uint CumulativeDistributionFunction<XType, PdfType>::capacity() const noexcept
{
  return cast<uint>(capacity_);
}

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
const XType& CumulativeDistributionFunction<XType, PdfType>::inverseFunction(
    const PdfType y) const noexcept
{
  ZISC_ASSERT(isInClosedBounds(y, 0.0, 1.0), "y is out of range: [0, 1].");
  const auto position = searchBinaryTree(yBegin(), yEnd(), y);
  const auto index = std::distance(yBegin(), position);
  return x_list_[index];
}

/*!
  */
template <typename XType, typename PdfType> inline
uint CumulativeDistributionFunction<XType, PdfType>::size() const noexcept
{
  return cast<uint>(size_);
}

/*!
  */
template <typename XType, typename PdfType> inline
void CumulativeDistributionFunction<XType, PdfType>::set(
    const std::vector<XType>& x_list,
    const std::vector<PdfType>& y_list) noexcept
{
  ZISC_ASSERT(x_list.size() == y_list.size(),
              "The sizes of the x and y list don't match.");
  setSize(cast<uint>(x_list.size()));

  for (uint index = 0; index < size(); ++index) {
    x_list_[index] = x_list[index];
    y_list_[index] = y_list[index];
  }

  initCdf();
}

/*!
  */
template <typename XType, typename PdfType> inline
void CumulativeDistributionFunction<XType, PdfType>::set(
    std::vector<XType>&& x_list,
    std::vector<PdfType>&& y_list) noexcept
{
  ZISC_ASSERT(x_list.size() == y_list.size(),
              "The sizes of the x and y list don't match.");
  setSize(cast<uint>(x_list.size()));

  for (uint index = 0; index < size(); ++index) {
    x_list_[index] = std::move(x_list[index]);
    y_list_[index] = std::move(y_list[index]);
  }

  initCdf();
}

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
XType* CumulativeDistributionFunction<XType, PdfType>::xList() noexcept
{
  return x_list_.get();
}

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
const XType* CumulativeDistributionFunction<XType, PdfType>::xList() const noexcept
{
  return x_list_.get();
}

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
PdfType* CumulativeDistributionFunction<XType, PdfType>::yList() noexcept
{
  return y_list_.get();
}

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType> inline
const PdfType* CumulativeDistributionFunction<XType, PdfType>::yList() const noexcept
{
  return y_list_.get();
}

/*!
  */
template <typename XType, typename PdfType> inline
void CumulativeDistributionFunction<XType, PdfType>::initCdf() noexcept
{
  // Type check
  static_assert(sizeof(uint32) <= sizeof(std::unique_ptr<XType[]>), "");
  static_assert(sizeof(std::unique_ptr<XType[]>) == sizeof(XType*), "");
  static_assert(sizeof(std::unique_ptr<PdfType[]>) == sizeof(PdfType*), "");

  // Calculate the CDF
  CompensatedSummation<PdfType> pdf_sum{0.0};
  for (uint index = 0; index < size(); ++index) {
    const auto pdf = y_list_[index];
    y_list_[index] = pdf_sum.get();
    pdf_sum.add(pdf);
  }
  ZISC_ASSERT(isInBounds(pdf_sum.get(), 1.0-1.0e-6, 1.0+1.0e-6),
              "The sum of the pdf list isn't 1.");

  // Make CDF arrays
  toBinaryTree(xBegin(), xEnd());
  toBinaryTree(yBegin(), yEnd());
}

/*!
  */
template <typename XType, typename PdfType> inline
void CumulativeDistributionFunction<XType, PdfType>::setSize(const uint list_size)
    noexcept
{
  if (capacity() < list_size) {
    x_list_ = std::make_unique<XType[]>(cast<std::size_t>(list_size));
    y_list_ = std::make_unique<PdfType[]>(cast<std::size_t>(list_size));
    capacity_ = list_size;
  }
  size_ = list_size;
}

/*!
  */
template <typename XType, typename PdfType> inline
XType* CumulativeDistributionFunction<XType, PdfType>::xBegin() noexcept
{
  return xList();
}

/*!
  */
template <typename XType, typename PdfType> inline
const XType* CumulativeDistributionFunction<XType, PdfType>::xBegin() const noexcept
{
  return xList();
}

/*!
  */
template <typename XType, typename PdfType> inline
XType* CumulativeDistributionFunction<XType, PdfType>::xEnd() noexcept
{
  return xList() + size();
}

/*!
  */
template <typename XType, typename PdfType> inline
const XType* CumulativeDistributionFunction<XType, PdfType>::xEnd() const noexcept
{
  return xList() + size();
}

/*!
  */
template <typename XType, typename PdfType> inline
PdfType* CumulativeDistributionFunction<XType, PdfType>::yBegin() noexcept
{
  return yList();
}

/*!
  */
template <typename XType, typename PdfType> inline
const PdfType* CumulativeDistributionFunction<XType, PdfType>::yBegin() const noexcept
{
  return yList();
}

/*!
  */
template <typename XType, typename PdfType> inline
PdfType* CumulativeDistributionFunction<XType, PdfType>::yEnd() noexcept
{
  return yList() + size();
}

/*!
  */
template <typename XType, typename PdfType> inline
const PdfType* CumulativeDistributionFunction<XType, PdfType>::yEnd() const noexcept
{
  return yList() + size();
}

} // namespace zisc

#endif // ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_INL_HPP
