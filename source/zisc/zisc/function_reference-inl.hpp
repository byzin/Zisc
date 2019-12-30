/*!
  \file function_reference-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUNCTION_REFERENCE_INL_HPP
#define ZISC_FUNCTION_REFERENCE_INL_HPP

#include "function_reference.hpp"
// Standard C++ library
#include <type_traits>
// Zisc
#include "error.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace zisc {

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
FunctionReference<ReturnT (ArgumentTypes...)>::FunctionReference() noexcept
{
}

template <typename ReturnT, typename ...ArgumentTypes>
template <typename Function> inline
FunctionReference<ReturnT (ArgumentTypes...)>::FunctionReference(
    Function&& func,
    EnableIf<kIsInvocableRaw<Function>>) noexcept
{
  initialize<Function>(std::forward<Function>(func));
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::operator()(
    ArgTypes&&... arguments) const noexcept -> ReturnType
{
  if constexpr (std::is_void_v<ReturnType>)
    invoke(std::forward<ArgTypes>(arguments)...);
  else
    return invoke(std::forward<ArgTypes>(arguments)...);
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::assign(
    const FunctionReference& other) noexcept -> FunctionReference&
{
  memory_ = other.memory_;
  callback_ = other.callback_;
  return *this;
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename Function> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::assign(
    Function&& func,
    EnableIf<kIsInvocableRaw<Function>>) noexcept -> FunctionReference&
{
  initialize<Function>(std::forward<Function>(func));
  return *this;
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
void FunctionReference<ReturnT (ArgumentTypes...)>::clear() noexcept
{
  callback_ = nullptr;
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::invoke(
    ArgTypes&&... arguments) const noexcept -> ReturnType
{
  ZISC_ASSERT(cast<bool>(*this), "This function reference is invalid.");
  if constexpr (std::is_void_v<ReturnType>)
    callback_(memory(), std::forward<ArgTypes>(arguments)...);
  else
    return callback_(memory(), std::forward<ArgTypes>(arguments)...);
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
void FunctionReference<ReturnT (ArgumentTypes...)>::swap(
    FunctionReference& other) noexcept
{
  {
    auto tmp = other.memory_;
    other.memory_ = memory_;
    memory_ = tmp;
  }
  {
    auto tmp = other.callback_;
    other.callback_ = callback_;
    callback_ = tmp;
  }
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename Function> inline
void FunctionReference<ReturnT (ArgumentTypes...)>::initialize(
    Function&& func) noexcept
{
  using Func = std::remove_volatile_t<std::remove_reference_t<Function>>;
  constexpr bool is_function_pointer = std::is_pointer_v<Func>;
  constexpr bool is_fanctor = std::is_object_v<Func>;
  if constexpr (is_function_pointer) {
    // Function pointer case
    static_assert(sizeof(Func) <= sizeof(Memory),
                  "the size of Function is larger than the size of a memory.");
    ::new (memory()) Func{func};
    callback_ = &invokeFunctionPointer<Func>;
  }
  else if constexpr (is_fanctor) {
    // Functor case
    static_assert(sizeof(&func) <= sizeof(Memory),
                  "the size of Function ref is larger than the size of a memory.");
    ::new (memory()) const void*{&func};
    callback_ = &invokeFunctor<Func>;
  }
  static_assert(is_function_pointer || is_fanctor, "Unsupported function type.");
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename FuncPointer> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::invokeFunctionPointer(
    const void* function_ptr,
    ArgumentTypes... arguments) noexcept -> ReturnType
{
  const auto ptr = cast<const FuncPointer*>(function_ptr);
  if constexpr (std::is_void_v<ReturnType>)
    std::invoke(*ptr, arguments...);
  else
    return std::invoke(*ptr, arguments...);
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename Functor> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::invokeFunctor(
    const void* functor,
    ArgumentTypes... arguments) noexcept -> ReturnType
{
  const auto ptr = cast<const Functor* const*>(functor);
  if constexpr (std::is_void_v<ReturnType>)
    std::invoke(*(*ptr), arguments...);
  else
    return std::invoke(*(*ptr), arguments...);
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
void* FunctionReference<ReturnT (ArgumentTypes...)>::memory() noexcept
{
  return &memory_;
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
const void* FunctionReference<ReturnT (ArgumentTypes...)>::memory() const noexcept
{
  return &memory_;
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
void swap(FunctionReference<ReturnT (ArgumentTypes...)>& lhs,
          FunctionReference<ReturnT (ArgumentTypes...)>& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace zisc

#endif // ZISC_FUNCTION_REFERENCE_INL_HPP
