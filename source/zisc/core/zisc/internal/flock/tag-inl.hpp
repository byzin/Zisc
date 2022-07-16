/*!
  \file tag-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_TAG_INL_HPP
#define ZISC_FLOCK_TAG_INL_HPP

#include "tag.hpp"
// Standard C++ library
#include <atomic>
#include <cstddef>
#include <memory>
#include <span>
#include <vector>
// Zisc
#include "log.hpp"
#include "worker_info.hpp"
#include "write_announcements.hpp"
#include "zisc/bit.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \param [in] old_value No description.
  \param [in] new_value No description.
  \return No description
  */
template <typename Type> inline
auto Tag<Type>::add(const IT old_value, const IT new_value) noexcept -> IT
{
  const IT v = new_value | cast<IT>(old_value & cntMask());
  return v;
}

/*!
  \details a safe cas that assigns the new value a tag that no concurrent cas
  on the same location has in its old value

  \param [in,out] loc No description.
  \param [in] old_value No description.
  \param [in] value No description.
  \param [in,out] write_announcements No description.
  \param [in,out] log No description.
  \param [in] aba_free No description.
  \return No description
  */
template <typename Type> inline
bool Tag<Type>::cas(std::atomic<IT>& loc,
                    IT old_value,
                    ConstReference value,
                    WriteAnnouncements* write_announcements,
                    Log* log,
                    const bool aba_free) noexcept
{
  bool result = false;
  if (log->isEmpty() || aba_free) {
    const IT new_value = next(old_value, value, bit_cast<IT>(&loc));
    result = loc.compare_exchange_strong(old_value,
                                         new_value,
                                         std::memory_order::acq_rel,
                                         std::memory_order::acquire);
  }
  else {
    // Announce the location and tag been written
    write_announcements->set(add(old_value, bit_cast<IT>(&loc)));
    log->skipIfDone([&loc, &old_value, &value, &result]()
    // skiip both for correctness, and efficiency
    {
      const IT new_value = next(old_value, value, bit_cast<IT>(&loc));
      result = loc.compare_exchange_strong(old_value,
                                           new_value,
                                           std::memory_order::acq_rel,
                                           std::memory_order::acquire);
    });
    // Unannounce the location
    write_announcements->clear();
  }
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr auto Tag<Type>::cntBit() noexcept -> IT
{
  const IT bit = 1ull << (64 - tagBits() + 1);
  return bit;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr auto Tag<Type>::cntMask() noexcept -> IT
{
  const IT mask = ~dataMask();
  return mask;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr auto Tag<Type>::dataMask() noexcept -> IT
{
  const IT mask = panicBit() - 1;
  return mask;
}

/*!
  \details No detailed description

  \param [in] old_value No description.
  \return No description
  */
template <typename Type> inline
auto Tag<Type>::inc(const IT old_value) noexcept -> IT
{
  const IT new_count = (old_value & cntMask()) + cntBit();
  return (new_count == 0) ? cntBit() : new_count; // avoid using 0
}

/*!
  \details No detailed description

  \param [in] old_value No description.
  \return No description
  */
template <typename Type> inline
auto Tag<Type>::init(ConstReference v) noexcept -> IT
{
  const IT new_v = cntBit() | bit_cast<IT>(v);
  return new_v;
}

/*!
  \details No detailed description

  \param [in] old_value No description.
  \param [in] new_value No description.
  \return No description
  */
template <typename Type> inline
auto Tag<Type>::next(const IT old_value, ConstReference new_value) noexcept -> IT
{
  const IT v = cast<IT>(new_value) | inc(old_value);
  return v;
}

/*!
  \details No detailed description

  \param [in] old_value No description.
  \param [in] new_value No description.
  \return No description
  */
template <typename Type> inline
auto Tag<Type>::next(const IT old_value,
                     ConstReference new_value,
                     const IT addr,
                     WriteAnnouncements* write_announcements) noexcept -> IT
{
  IT new_count = inc(old_value);
  bool panic = false;
  if (((old_value & topBit()) != (new_count & topBit())) || // overflow, unlikely
      ((old_value & panicBit()) != 0)) [[unlikely]] { // panic bit set, unlikely
    for (const IT ann : write_announcements->scan()) { // check if we have to panic
      if ((ann & dataMask()) == (addr & dataMask()) && // same mutable_val obj
          (ann & topBit()) == (new_count & topBit()) &&
          (ann & cntMask()) >= (new_count & cntMask() & ~panicBit())) {
        panic = true;
        break;
      }
    }
  }

  IT value = bit_cast<IT>(new_value) | (new_count & ~panicBit());
  if (panic) [[unlikely]] {
    pmr::vector<IT> announced_tags = write_announcements->scan();
    while (true) { // loop until new_count is not announced
      bool announced = false;
      for (const IT ann : announced_tags) {
        if ((ann & dataMask()) == (addr & dataMask()) && // same mutable_val obj
            (ann & cntMask()) == new_count) {
          announced = true;
          break;
        }
      }
      if (!announced) {
        value = bit_cast<IT>(new_value) | (new_count | panicBit());
        break;
      }
      new_count = inc(new_count);
    }
  }
  return value;
}
/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr auto Tag<Type>::panicBit() noexcept -> IT
{
  const IT bit = 1ull << (64 - tagBits());
  return bit;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr std::size_t Tag<Type>::tagBits() noexcept
{
  const std::size_t bits = 16; // number of bits to use for tag (including panic bit)
  return bits;
}

/*!
  \details No detailed description

  \param [in] old_value No description.
  \param [in] new_value No description.
  \return No description
  */
template <typename Type> inline
Type Tag<Type>::value(const IT value) noexcept
{
  const auto v = bit_cast<ValueT>(value & dataMask());
  return v;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type> inline
constexpr auto Tag<Type>::topBit() noexcept -> IT
{
  const IT bit = 1ull << 63;
  return bit;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_TAG_INL_HPP */
