/*!
  \file logger.hpp
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

#ifndef ZISC_LOGGER_HPP
#define ZISC_LOGGER_HPP

// Standard C++ library
#include <array>
#include <bitset>
#include <cstdint>
#include <limits>
#include <ostream>
#include <source_location>
#include <string_view>
// Zisc
#include "non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class Logger : private NonCopyable<Logger>
{
 public:
  /*!
    \brief No brief description

    No detailed description.
    */
  enum class Severity : uint64b
  {
    kError = 0b01 << 0,
    kWarning = 0b01 << 1,
    kInfo = 0b01 << 2,
    kDebug = 0b01 << 3
  };


  // Type aliases
  using BufferT = std::array<char, 1024>; //!< The type used for message buffer


  //! Create an empty logger
  Logger() noexcept = default;

  //! Create a logger
  Logger(std::ostream* log) noexcept;

  //! Move a data
  Logger(Logger&& other) noexcept;


  //! Move a data
  auto operator=(Logger&& other) noexcept -> Logger&;


  //! Check if the given severity is enabled
  auto isSeverityEnabled(const Severity severity) const noexcept -> bool;

  //! Log the given message
  void log(const Severity severity,
           const std::string_view message, 
           const std::source_location& location = std::source_location::current()) const noexcept;

  //! Return the maximum length of log message
  static constexpr auto messageLengthLimit() noexcept -> std::size_t;

  //! Set the given severity enabled if we give the flag with true
  void setSeverity(const Severity severity, const bool flag = true) noexcept;

  //! Check if the logger has an underlying log stream
  auto hasStream() const noexcept -> bool;

  //! Set the internal log stream
  void setStream(std::ostream* log) noexcept;

 private:
  //! Create a log message from the given info
  [[nodiscard]]
  static auto createMessage(const Severity severity,
                            const std::string_view message,
                            const std::source_location& location,
                            BufferT* buffer) noexcept -> int;

  //! Return the text of the given severity level
  static constexpr auto getSeverityText(const Severity severity) noexcept -> std::string_view;

  //! Return the underlying log stream
  auto logStream() const noexcept -> std::ostream&;


  std::ostream* log_ = nullptr;
  std::bitset<64> filter_{(std::numeric_limits<unsigned long long>::max)()};
};

} /* namespace zisc */

#include "logger-inl.hpp"

#endif /* ZISC_LOGGER_HPP */
