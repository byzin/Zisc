/*!
  \file non_copyable.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_NON_COPYABLE_HPP
#define ZISC_NON_COPYABLE_HPP

namespace zisc {

/*!
 \brief A class that derives from this class can't be copied or assigned
 \details
 No detailed.
 */
template <typename Type>
class NonCopyable
{
 protected:
  NonCopyable() {}
  ~NonCopyable() {}

 private:
  //! Prohibit copy constructor
  NonCopyable(const NonCopyable&) = delete;

  //! Prohibit copy assignment
  NonCopyable& operator=(const NonCopyable&) = delete;

  //! Prohibit copy assignment
  Type& operator=(const Type&) = delete;
};

} // namespace zisc

#endif // ZISC_NON_COPYABLE_HPP
