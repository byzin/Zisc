/*!
  \file non_copyable.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_NON_COPYABLE_HPP
#define ZISC_NON_COPYABLE_HPP

namespace zisc {

/*!
  \brief A class that derives from this class can't be copied or assigned

  No detailed description.

  \tparam Type No description.
  */
template <typename Type>
class NonCopyable
{
 public:
  //! Prohibit copy constructor
  NonCopyable(const NonCopyable&) = delete;

  //! Prohibit copy assignment
  auto operator=(const NonCopyable&) -> NonCopyable& = delete;

  //! Prohibit copy assignment
  auto operator=(const Type&) -> Type& = delete;

 protected:
  //
  NonCopyable() noexcept = default;

  //
  ~NonCopyable() noexcept = default;
};

} // namespace zisc

#endif // ZISC_NON_COPYABLE_HPP
