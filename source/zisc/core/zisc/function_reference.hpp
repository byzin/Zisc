/*!
  \file function_reference.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUNCTION_REFERENCE_HPP
#define ZISC_FUNCTION_REFERENCE_HPP

// Standard C++ library
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <type_traits>

namespace zisc {

//! A reference to a function
template <typename> class FunctionReference;

/*!
  \brief A reference to a function

  No detailed description.

  \tparam ReturnT No description.
  \tparam ArgTypes No description.
  */
template <typename ReturnT, typename ...ArgTypes>
class FunctionReference<ReturnT (ArgTypes...)>
{
 public:
  // Type aliases
  using FunctionPointer = ReturnT (*)(ArgTypes...);


  //! Create an empry
  FunctionReference() noexcept = default;

  //! Create a reference to the given callable object
  template <std::invocable<ArgTypes...> Func>
  explicit FunctionReference(Func&& func) noexcept;


  //! Create a reference to the given callable object
  template <std::invocable<ArgTypes...> Func>
  auto operator=(Func&& func) noexcept -> FunctionReference&;

  //! Invoke a referenced callable object
  template <typename ...Args>
  auto operator()(Args&&... args) const -> ReturnT
  requires std::invocable<ReturnT (*)(ArgTypes...), Args...>;

  //! Check whether this refers a callable object 
  explicit operator bool() const noexcept;


  //! Create a reference to the given callable object
  template <std::invocable<ArgTypes...> Func>
  auto assign(Func&& func) noexcept -> FunctionReference&;

  //! Clear the underlying reference to a callable object
  void clear() noexcept;

  //! Invoke a referenced callable object
  template <typename ...Args>
  auto invoke(Args&&... args) const -> ReturnT
  requires std::invocable<ReturnT (*)(ArgTypes...), Args...>;

  //! Exchange referenced callable objects of this and other
  void swap(FunctionReference& other) noexcept;

 private:
  // Type aliases
  using FuncRefMemory = std::aligned_storage_t<sizeof(void*), alignof(void*)>;
  using InvokerPointer = ReturnT (*)(FuncRefMemory, ArgTypes...);


  //! Initialize with a callable object
  template <std::invocable<ArgTypes...> Func>
  void initialize(Func&& func) noexcept;

  //! Invoke a referenced callable object 
  template <typename FuncPtr>
  static auto invokeFunc(FuncRefMemory mem, ArgTypes... args) -> ReturnT;

  //! Return the underlying invoker pointer
  auto invoker() const noexcept -> const InvokerPointer&;

  //! Return the memory of the function reference
  [[nodiscard]]
  auto memory() noexcept -> FuncRefMemory&;

  //! Return the memory of the function reference
  [[nodiscard]]
  auto memory() const noexcept -> const FuncRefMemory&;


  FuncRefMemory memory_{};
  InvokerPointer invoker_ = nullptr;
};

//! Swap memories in the given instances
template <typename ReturnT, typename ...ArgTypes>
void swap(FunctionReference<ReturnT (ArgTypes...)>& lhs,
          FunctionReference<ReturnT (ArgTypes...)>& rhs) noexcept;

} // namespace zisc

#include "function_reference-inl.hpp"

#endif // ZISC_FUNCTION_REFERENCE_HPP
