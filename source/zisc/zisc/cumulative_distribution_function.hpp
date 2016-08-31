/*!
  \file cumulative_distribution_function.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP
#define ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP

// Standard C++ library
#include <vector>
// Zisc
#include "type_traits.hpp"

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
  //! Create a CDF and move x and pdf data to CDF
  template <typename XIterator, typename PdfIterator>
  CumulativeDistributionFunction(XIterator x_begin,
                                 XIterator x_end,
                                 PdfIterator pdf_begin,
                                 PdfIterator pdf_end) noexcept;

  //! Move data
  CumulativeDistributionFunction(CumulativeDistributionFunction&& other) noexcept;


  //! Move data
  CumulativeDistributionFunction& operator=(
      CumulativeDistributionFunction&& other) noexcept;


  //! Return the x value associated with the y value
  const XType& inverseFunction(const PdfType& y) const noexcept;

  //! Return the x list
  const std::vector<XType>& xList() const noexcept;

  //! Return the y list
  const std::vector<PdfType>& yList() const noexcept;

 private:
  //! Create a CDF and move x and pdf data to CDF
  template <typename XIterator, typename PdfIterator>
  bool initialize(XIterator x_begin,
                  XIterator x_end,
                  PdfIterator pdf_begin,
                  PdfIterator pdf_end) noexcept;


  std::vector<XType> x_list_;
  std::vector<PdfType> y_list_;
};

} // namespace zisc

#include "cumulative_distribution_function-inl.hpp"

#endif // ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP
