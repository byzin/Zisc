/*!
  \file cumulative_distribution_function.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP
#define ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP

// Standard C++ library
#include <cstddef>
#include <memory>
#include <vector>
// Zisc
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename XType, typename PdfType>
class CumulativeDistributionFunction
{
  static_assert(kIsFloat<PdfType>, "PdfType isn't floating point type.");

 public:
  //! Create a CDF
  CumulativeDistributionFunction(const std::vector<XType>& x_list,
                                 const std::vector<PdfType>& y_list) noexcept;

  //! Create a CDF
  CumulativeDistributionFunction(std::vector<XType>&& x_list,
                                 std::vector<PdfType>&& y_list) noexcept;

  //! Move data
  CumulativeDistributionFunction(CumulativeDistributionFunction&& other) noexcept;


  //! Move data
  CumulativeDistributionFunction& operator=(
      CumulativeDistributionFunction&& other) noexcept;


  //! Return the size of the capacity
  uint capacity() const noexcept;

  //! Return the x value associated with the y value
  const XType& inverseFunction(const PdfType y) const noexcept;

  //! Return the size of the lists
  uint size() const noexcept;

  //! Set a CDF data
  void set(const std::vector<XType>& x_list,
           const std::vector<PdfType>& y_list) noexcept;

  //! Set a CDF data
  void set(std::vector<XType>&& x_list,
           std::vector<PdfType>&& y_list) noexcept;

  //! Return the x list
  XType* xList() noexcept;

  //! Return the x list
  const XType* xList() const noexcept;

  //! Return the y list
  PdfType* yList() noexcept;

  //! Return the y list
  const PdfType* yList() const noexcept;

 private:
  //! Initialize the CDF
  void initCdf() noexcept;

  //! Set the size of the list
  void setSize(const uint list_size) noexcept;

  //! Return the begin of the x list
  XType* xBegin() noexcept;

  //! Return the begin of the x list
  const XType* xBegin() const noexcept;

  //! Return the end of the x list
  XType* xEnd() noexcept;

  //! Return the end of the x list
  const XType* xEnd() const noexcept;

  //! Return the begin of the y list
  PdfType* yBegin() noexcept;

  //! Return the begin of the y list
  const PdfType* yBegin() const noexcept;

  //! Return the end of the y list
  PdfType* yEnd() noexcept;

  //! Return the end of the y list
  const PdfType* yEnd() const noexcept;


  std::unique_ptr<XType[]> x_list_;
  std::unique_ptr<PdfType[]> y_list_;
  uint32 size_;
  uint32 capacity_;
};

} // namespace zisc

#include "cumulative_distribution_function-inl.hpp"

#endif // ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP
