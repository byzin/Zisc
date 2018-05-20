/*!
  \file function_reference-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
FunctionReference<ReturnT (ArgumentTypes...)>::FunctionReference(
    FunctionPointer function_ptr) noexcept
{
  initFunctionPointer(function_ptr);
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename RType, typename ...ArgTypes> inline
FunctionReference<ReturnT (ArgumentTypes...)>::FunctionReference(
    RType (*function_ptr)(ArgTypes...),
    EnableIf<is_invocable_v<RType (*)(ArgTypes...),
                                 ArgumentTypes...>>) noexcept
{
  initFunctionPointer(function_ptr);
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename Functor> inline
FunctionReference<ReturnT (ArgumentTypes...)>::FunctionReference(
    const Functor& functor,
    EnableIf<is_invocable_v<Functor, ArgumentTypes...>>) noexcept
{
  initFunctor(functor);
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::operator()(
    ArgTypes&&... arguments) const noexcept -> ReturnType
{
  return invoke(std::forward<ArgTypes>(arguments)...);
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::assign(
    FunctionPointer function_ptr) noexcept -> FunctionReference&
{
  initFunctionPointer(function_ptr);
  return *this;
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename RType, typename ...ArgTypes> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::assign(
    RType (*function_ptr)(ArgTypes...),
    EnableIf<is_invocable_v<RType (*)(ArgTypes...),
                                 ArgumentTypes...>>) noexcept -> FunctionReference&
{
  initFunctionPointer(function_ptr);
  return *this;
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename Functor> inline
auto FunctionReference<ReturnT (ArgumentTypes...)>::assign(
    const Functor& functor,
    EnableIf<is_invocable_v<Functor, ArgumentTypes...>>) noexcept
        -> FunctionReference&
{
  initFunctor(functor);
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
template <typename FuncPointer> inline
void FunctionReference<ReturnT (ArgumentTypes...)>::initFunctionPointer(
    FuncPointer function_ptr) noexcept
{
  static_assert(sizeof(FuncPointer) <= kStorageSize,
                "the size of FuncPointer is larger than the size of a memory.");
  ZISC_ASSERT(function_ptr != nullptr, "The function pointer is null.");
  ::new (memory()) FuncPointer{function_ptr};
  callback_ = &invokeFunctionPointer<FuncPointer>;
}

/*!
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename Functor> inline
void FunctionReference<ReturnT (ArgumentTypes...)>::initFunctor(
    const Functor& functor) noexcept
{
  ::new (memory()) const void*{&functor};
  callback_ = &invokeFunctor<Functor>;
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
  return (*ptr)(static_cast<ArgumentTypes>(arguments)...);
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
  return (*(*ptr))(static_cast<ArgumentTypes>(arguments)...);
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
