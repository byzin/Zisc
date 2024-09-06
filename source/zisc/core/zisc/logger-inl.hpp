/*!
  \file logger-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \note No notation.
  \attention No attention.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOGGER_INL_HPP
#define ZISC_LOGGER_INL_HPP

#include "logger.hpp"
// Standard C++ library
#include <array>
#include <bitset>
#include <chrono>
#include <cstdio>
#include <ostream>
#include <source_location>
#include <string_view>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [out] log No description.
  */
inline
Logger::Logger(std::ostream* log) noexcept
{
  static_assert(sizeof(filter_) == sizeof(uint64b));
  setStream(log);
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Logger::Logger(Logger&& other) noexcept :
    log_{other.log_},
    filter_{std::move(other).filter_}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
auto Logger::operator=(Logger&& other) noexcept -> Logger&
{
  log_ = other.log_;
  filter_ = std::move(other).filter_;
  return *this;
}

/*!
  \details No detailed description

  \param [in] severity No description.
  \return No description
  */
inline
auto Logger::isSeverityEnabled(const Severity severity) const noexcept -> bool
{
  const bool result = filter_.test(static_cast<std::size_t>(severity));
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr auto Logger::messageLengthLimit() noexcept -> std::size_t
{
  constexpr std::size_t n = sizeof(BufferT) / sizeof(BufferT::value_type);
  return n;
}

/*!
  \details No detailed description

  \param [in] severity No description.
  \param [in] message No description.
  \param [in] location No description.
  */
inline
void Logger::log(const Severity severity,
                 const std::string_view message,
                 const std::source_location& location) const noexcept
{
  if (!hasStream() || !isSeverityEnabled(severity))
    return;

  // Output message
  BufferT log_message{};
  int result = createMessage(severity, message, location, &log_message);
  logStream() << log_message.data();
  if (result < 0) [[unlikely]] {
    constexpr std::string_view text{"There was something error at the previous message creation"};
    result = createMessage(Severity::kWarning, text, location, &log_message);
    logStream() << log_message.data();
  }
}

/*!
  \details No detailed description

  \param [in] severity No description.
  \param [in] flag No description.
  */
inline
void Logger::setSeverity(const Severity severity, const bool flag) noexcept
{
  filter_.set(static_cast<std::size_t>(severity), flag);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Logger::hasStream() const noexcept -> bool
{
  const bool result = log_ != nullptr;
  return result;
}

/*!
  \details No detailed description

  \param [out] log No description.
  */
inline
void Logger::setStream(std::ostream* log) noexcept
{
  log_ = log;
}

/*!
  \details No detailed description

  \param [in] severity No description.
  \param [in] message No description.
  \param [in] location No description.
  \param [out] buffer No description.
  \return No description
  */
inline
auto Logger::createMessage(const Severity severity,
                           const std::string_view message,
                           const std::source_location& location,
                           BufferT* buffer) noexcept -> int 
{
  // Severity text
  const std::string_view severity_text = getSeverityText(severity);

  // Date text
  const std::chrono::time_point now = std::chrono::system_clock::now();
  const std::chrono::time_point days = std::chrono::floor<std::chrono::days>(now);
  const std::chrono::year_month_day ymd{days};
  const std::chrono::hh_mm_ss hms{now - days};

  const int result = std::snprintf(buffer->data(),
                                   buffer->max_size(), 
                                   "[%7s %04d-%02u-%02u %02d:%02d:%02d.%06d    %s:%u] %s\n",
                                   severity_text.data(),
                                   static_cast<int>(ymd.year()),
                                   static_cast<unsigned>(ymd.month()),
                                   static_cast<unsigned>(ymd.day()),
                                   static_cast<int>(hms.hours().count()),
                                   static_cast<int>(hms.minutes().count()),
                                   static_cast<int>(hms.seconds().count()),
                                   static_cast<int>(hms.subseconds().count()),
                                   location.file_name(),
                                   static_cast<unsigned>(location.line()),
                                   message.data());
  return result;
}

/*!
  \details No detailed description

  \param [in] severity No description.
  \return No description
  */
inline
constexpr auto Logger::getSeverityText(const Severity severity) noexcept -> std::string_view
{
  std::string_view text{};
  switch (severity) {
   case Severity::kError: {
    text = "Error";
    break;
   }
   case Severity::kWarning: {
    text = "Warning";
    break;
   }
   case Severity::kInfo: {
    text = "Info";
    break;
   }
   case Severity::kDebug: {
    text = "Debug";
    break;
   }
  }
  return text;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Logger::logStream() const noexcept -> std::ostream&
{
  return *log_;
}

} /* namespace zisc */

#endif /* ZISC_LOGGER_INL_HPP */
