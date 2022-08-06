/*!
  \file function_reference-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
  assign(func);
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
  return assign(std::forward<Func>(func));
}

/*!
  \details No detailed description

  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgTypes...)>::operator()(ArgTypes... args) const -> ReturnT
{
  return invoke(forward<ArgTypes>(args)...);
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

  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgTypes...)>::invoke(ArgTypes... args) const -> ReturnT
{
  ZISC_ASSERT(cast<bool>(*this), "Underlying reference is invalid.");
  return invoker()(memory(), forward<ArgTypes>(args)...);
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

  \tparam Type No description.
  \param [in] arg No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> template <typename Type> inline
constexpr Type FunctionReference<ReturnT (ArgTypes...)>::forward(ArgRef<Type> arg) noexcept
{
  if constexpr (std::is_lvalue_reference_v<Type>)
    return arg;
  else
    return std::move(arg);
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
  using Function = std::remove_volatile_t<std::remove_reference_t<Func>>;
  constexpr bool is_func_ptr = std::is_pointer_v<Function>;
  constexpr bool has_func_ptr = std::is_convertible_v<Function, FunctionPointer>;
  constexpr bool is_fanctor = std::is_object_v<Function>;
  if constexpr (is_func_ptr) {
    // Function pointer case
    using FuncPtr = Function;
    static_assert(sizeof(FuncPtr) == sizeof(FuncRefMemory),
                  "The memory doesn't have enough space for the function ptr.");
    static_assert(std::alignment_of_v<FuncPtr> <= std::alignment_of_v<FuncRefMemory>,
                  "The memory doesn't satify the alignment of the function ptr.");
    ::new (std::addressof(memory())) FuncPtr{func};
    invoker_ = std::addressof(invokeFunctionPointer<Function>);
  }
  else if constexpr (has_func_ptr) {
    // Function case
    using FuncPtr = FunctionPointer;
    static_assert(sizeof(FuncPtr) == sizeof(FuncRefMemory),
                  "The memory doesn't have enough space for the function ptr.");
    static_assert(std::alignment_of_v<FuncPtr> <= std::alignment_of_v<FuncRefMemory>,
                  "The memory doesn't satify the alignment of the function ptr.");
    ::new (std::addressof(memory())) FuncPtr{func};
    invoker_ = std::addressof(invokeFunctionPointer<FunctionPointer>);
  }
  else if constexpr (is_fanctor) {
    // Functor case
    using FuncPtr = std::add_pointer_t<Function>;
    static_assert(sizeof(FuncPtr) == sizeof(FuncRefMemory),
                  "The memory doesn't have enough space for the functor ptr.");
    static_assert(std::alignment_of_v<FuncPtr> <= std::alignment_of_v<FuncRefMemory>,
                  "The memory doesn't satify the alignment of the functor ptr.");
    ::new (std::addressof(memory())) FuncPtr{std::addressof(func)};
    invoker_ = std::addressof(invokeFunctor<Function>);
  }
  static_assert(is_func_ptr || has_func_ptr || is_fanctor, "Unsupported func type.");
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
auto FunctionReference<ReturnT (ArgTypes...)>::invokeFunctionPointer(
    FuncRefMemory mem,
    ArgTypes... args) -> ReturnT
{
  auto ptr = zisc::bit_cast<FuncPtr>(mem);
  if constexpr (std::is_void_v<ReturnT>)
    std::invoke(ptr, forward<ArgTypes>(args)...);
  else
    return std::invoke(ptr, forward<ArgTypes>(args)...);
}

/*!
  \details No detailed description

  \tparam Functor No description.
  \param [in] mem No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> template <typename Functor> inline
auto FunctionReference<ReturnT (ArgTypes...)>::invokeFunctor(
    FuncRefMemory mem,
    ArgTypes... args) -> ReturnT
{
  using FuncPtr = std::add_pointer_t<Functor>;
  auto ptr = zisc::bit_cast<FuncPtr>(mem);
  if constexpr (std::is_void_v<ReturnT>)
    std::invoke(*ptr, forward<ArgTypes>(args)...);
  else
    return std::invoke(*ptr, forward<ArgTypes>(args)...);
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
