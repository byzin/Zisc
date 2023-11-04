/*!
  \file function_reference-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUNCTION_REFERENCE_INL_HPP
#define ZISC_FUNCTION_REFERENCE_INL_HPP

#include "function_reference.hpp"
// Standard C++ library
#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "bit.hpp"
#include "concepts.hpp"
#include "error.hpp"
#include "utility.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] func No description.
  */
template <typename ReturnT, typename ...ArgTypes>
template <std::invocable<ArgTypes...> Func> inline
FunctionReference<ReturnT (ArgTypes...)>::FunctionReference(Func&& func) noexcept
{
  assign(std::forward<Func>(func));
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] func No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <std::invocable<ArgTypes...> Func> inline
auto FunctionReference<ReturnT (ArgTypes...)>::operator=(Func&& func) noexcept -> FunctionReference&
{
  assign(std::forward<Func>(func));
  return *this;
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <typename ...Args> inline
auto FunctionReference<ReturnT (ArgTypes...)>::operator()(Args&&... args) const -> ReturnT
    requires std::invocable<ReturnT (*)(ArgTypes...), Args...>
{
  return invoke(std::forward<Args>(args)...);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
FunctionReference<ReturnT (ArgTypes...)>::operator bool() const noexcept
{
  const bool result = invoker() != nullptr;
  return result;
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] func No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <std::invocable<ArgTypes...> Func> inline
auto FunctionReference<ReturnT (ArgTypes...)>::assign(Func&& func) noexcept -> FunctionReference&
{
  if constexpr (std::same_as<FunctionReference, std::remove_cvref_t<Func>>) {
    memory_ = func.memory();
    invoker_ = func.invoker();
  }
  else {
    initialize<Func>(std::forward<Func>(func));
  }
  return *this;
}

/*!
  \details No detailed description
  */
template <typename ReturnT, typename ...ArgTypes> inline
void FunctionReference<ReturnT (ArgTypes...)>::clear() noexcept
{
  memory_ = FuncRefMemory{};
  invoker_ = nullptr;
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <typename ...Args> inline
auto FunctionReference<ReturnT (ArgTypes...)>::invoke(Args&&... args) const -> ReturnT
    requires std::invocable<ReturnT (*)(ArgTypes...), Args...>
{
  ZISC_ASSERT(static_cast<bool>(*this), "Underlying reference is invalid.");
  return invoker()(memory(), std::forward<Args>(args)...);
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <typename ReturnT, typename ...ArgTypes> inline
void FunctionReference<ReturnT (ArgTypes...)>::swap(FunctionReference& other) noexcept
{
  std::swap(memory_, other.memory_);
  std::swap(invoker_, other.invoker_);
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] func No description.
  */
template <typename ReturnT, typename ...ArgTypes>
template <std::invocable<ArgTypes...> Func> inline
void FunctionReference<ReturnT (ArgTypes...)>::initialize(Func&& func) noexcept
{
  using FunctionT = std::remove_volatile_t<std::remove_reference_t<Func>>;
  constexpr bool is_func_ptr = std::is_pointer_v<FunctionT>;
  constexpr bool has_func_ptr = std::is_convertible_v<FunctionT, FunctionPointer>;
  constexpr bool is_fanctor = std::is_object_v<FunctionT>;
  static_assert(is_func_ptr || has_func_ptr || is_fanctor, "Unsupported function type.");
  using FuncPtr = std::conditional_t<is_func_ptr,  FunctionT,
                  std::conditional_t<has_func_ptr, FunctionPointer,
                  std::conditional_t<is_fanctor,   std::add_pointer_t<FunctionT>,
                                                   void*>>>;
  static_assert(sizeof(FuncPtr) == sizeof(FuncRefMemory),
                "The memory doesn't have enough space for the function ptr.");
  static_assert(std::alignment_of_v<FuncPtr> <= std::alignment_of_v<FuncRefMemory>,
                "The memory doesn't satisfy the alignment of the function ptr.");

  Func f = std::forward<Func>(func);
  if constexpr (is_func_ptr || has_func_ptr)
    memory() = zisc::bit_cast<FuncRefMemory>(FuncPtr{f});
  else if constexpr (is_fanctor)
    memory() = zisc::bit_cast<FuncRefMemory>(FuncPtr{std::addressof(f)});
  invoker_ = std::addressof(invokeFunc<FuncPtr>);
}

/*!
  \details No detailed description

  \tparam FuncPtr No description.
  \param [in] mem No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <typename FuncPtr> inline
auto FunctionReference<ReturnT (ArgTypes...)>::invokeFunc(FuncRefMemory mem,
                                                          ArgTypes... args) -> ReturnT
{
  static_assert(std::is_pointer_v<FuncPtr>);
  auto* ptr = zisc::bit_cast<FuncPtr>(mem);
  return std::invoke(*ptr, std::forward<std::add_rvalue_reference_t<ArgTypes>>(args)...);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgTypes...)>::invoker() const noexcept
    -> const InvokerPointer&
{
  return invoker_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgTypes...)>::memory() noexcept -> FuncRefMemory&
{
  return memory_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgTypes...)>::memory() const noexcept ->const FuncRefMemory& 
{
  return memory_;
}

/*!
  \details No detailed description

  \tparam ReturnT No description.
  \tparam ArgTypes No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  */
template <typename ReturnT, typename ...ArgTypes> inline
void swap(FunctionReference<ReturnT (ArgTypes...)>& lhs,
          FunctionReference<ReturnT (ArgTypes...)>& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace zisc

#endif // ZISC_FUNCTION_REFERENCE_INL_HPP
