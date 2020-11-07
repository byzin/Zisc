/*!
  \file function_reference-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUNCTION_REFERENCE_INL_HPP
#define ZISC_FUNCTION_REFERENCE_INL_HPP

#include "function_reference.hpp"
// Standard C++ library
#include <memory>
#include <type_traits>
// Zisc
#include "concepts.hpp"
#include "error.hpp"
#include "utility.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename ReturnT, typename ...ArgTypes> inline
FunctionReference<ReturnT (ArgTypes...)>::FunctionReference() noexcept
{
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] func No description.
  */
template <typename ReturnT, typename ...ArgTypes>
template <InvocableR<ReturnT, ArgTypes...> Func> inline
FunctionReference<ReturnT (ArgTypes...)>::FunctionReference(Func&& func) noexcept
{
  assign(func);
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <typename ...Args> inline
auto FunctionReference<ReturnT (ArgTypes...)>::operator()(Args&&... args) const
    -> ReturnType
{
  static_assert(Invocable<FunctionPointer, Args...>,
                "This cannot be invoke with the args.");
  if constexpr (std::is_void_v<ReturnType>)
    invoke(std::forward<Args>(args)...);
  else
    return invoke(std::forward<Args>(args)...);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
FunctionReference<ReturnT (ArgTypes...)>::operator bool() const noexcept
{
  const bool result = callback_ != nullptr;
  return result;
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] func No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <InvocableR<ReturnT, ArgTypes...> Func> inline
auto FunctionReference<ReturnT (ArgTypes...)>::assign(Func&& func) noexcept
    -> FunctionReference&
{
  if constexpr (SameAs<FunctionReference, std::remove_cvref_t<Func>>) {
    memory_ = func.memory_;
    callback_ = func.callback_;
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
  callback_ = nullptr;
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <typename ...Args> inline
auto FunctionReference<ReturnT (ArgTypes...)>::invoke(Args&&... args) const
    -> ReturnType
{
  static_assert(Invocable<FunctionPointer, Args...>,
                "This cannot be invoke with the args.");
  ZISC_ASSERT(cast<bool>(*this), "Underlying reference is invalid.");
  if constexpr (std::is_void_v<ReturnType>)
    callback_(memory(), std::forward<Args>(args)...);
  else
    return callback_(memory(), std::forward<Args>(args)...);
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <typename ReturnT, typename ...ArgTypes> inline
void FunctionReference<ReturnT (ArgTypes...)>::swap(FunctionReference& other) noexcept
{
  zisc::swap(memory_, other.memory_);
  zisc::swap(callback_, other.callback_);
}

/*!
  \details No detailed description

  \tparam Func No description.
  \param [in] func No description.
  */
template <typename ReturnT, typename ...ArgTypes>
template <InvocableR<ReturnT, ArgTypes...> Func> inline
void FunctionReference<ReturnT (ArgTypes...)>::initialize(Func&& func) noexcept
{
  using Function = std::remove_volatile_t<std::remove_reference_t<Func>>;
  constexpr bool is_function_pointer = std::is_pointer_v<Function>;
  constexpr bool is_fanctor = std::is_object_v<Function>;
  if constexpr (is_function_pointer) {
    // Function pointer case
    static_assert(sizeof(Function) <= sizeof(Memory),
                  "The size of Function is larger than the size of a memory.");
    ::new (memory()) Function{func};
    callback_ = std::addressof(invokeFunctionPointer<Function>);
  }
  else if constexpr (is_fanctor) {
    // Functor case
    static_assert(sizeof(std::addressof(func)) <= sizeof(Memory),
                  "The size of Function ref is larger than the size of a memory.");
    ::new (memory()) const void*{std::addressof(func)};
    callback_ = std::addressof(invokeFunctor<Function>);
  }
  static_assert(is_function_pointer || is_fanctor, "Unsupported function type.");
}

/*!
  \details No detailed description

  \tparam FuncPointer No description.
  \param [in] func_ptr No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <typename FuncPointer> inline
auto FunctionReference<ReturnT (ArgTypes...)>::invokeFunctionPointer(
    const void* func_ptr,
    ArgTypes... args) -> ReturnType
{
  const auto ptr = cast<const FuncPointer*>(func_ptr);
  if constexpr (std::is_void_v<ReturnType>)
    std::invoke(*ptr, std::forward<ArgTypes>(args)...);
  else
    return std::invoke(*ptr, std::forward<ArgTypes>(args)...);
}

/*!
  \details No detailed description

  \tparam Functor No description.
  \param [in] func_ptr No description.
  \param [in] args No description.
  \return No description
  */
template <typename ReturnT, typename ...ArgTypes>
template <typename Functor> inline
auto FunctionReference<ReturnT (ArgTypes...)>::invokeFunctor(
    const void* func_ptr,
    ArgTypes... args) -> ReturnType
{
  const auto ptr = cast<const Functor* const*>(func_ptr);
  if constexpr (std::is_void_v<ReturnType>)
    std::invoke(*(*ptr), std::forward<ArgTypes>(args)...);
  else
    return std::invoke(*(*ptr), std::forward<ArgTypes>(args)...);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
void* FunctionReference<ReturnT (ArgTypes...)>::memory() noexcept
{
  return &memory_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgTypes> inline
const void* FunctionReference<ReturnT (ArgTypes...)>::memory() const noexcept
{
  return &memory_;
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
