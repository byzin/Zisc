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
template <typename ReturnType, typename ...ArgumentTypes> inline
FunctionReference<ReturnType (ArgumentTypes...)>::FunctionReference(
    FunctionPointer function_ptr) noexcept
{
  initFunctionPointer(function_ptr);
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes>
template <typename RType, typename ...ArgTypes> inline
FunctionReference<ReturnType (ArgumentTypes...)>::FunctionReference(
    RType (*function_ptr)(ArgTypes...),
    EnableIf<std::is_invocable_v<RType (*)(ArgTypes...),
                                 ArgumentTypes...>>) noexcept
{
  initFunctionPointer(function_ptr);
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes>
template <typename Functor> inline
FunctionReference<ReturnType (ArgumentTypes...)>::FunctionReference(
    const Functor& functor,
    EnableIf<std::is_invocable_v<Functor, ArgumentTypes...>>) noexcept
{
  initFunctor(functor);
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes>
template <typename ...ArgTypes> inline
ReturnType FunctionReference<ReturnType (ArgumentTypes...)>::operator()(
    ArgTypes&&... arguments) const noexcept
{
  return invoke(std::forward<ArgTypes>(arguments)...);
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes>
template <typename ...ArgTypes> inline
ReturnType FunctionReference<ReturnType (ArgumentTypes...)>::invoke(
    ArgTypes&&... arguments) const noexcept
{
  return callback_(memory(), std::forward<ArgTypes>(arguments)...);
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes>
template <typename FuncPointer> inline
void FunctionReference<ReturnType (ArgumentTypes...)>::initFunctionPointer(
    FuncPointer function_ptr) noexcept
{
  ::new (memory()) FuncPointer{function_ptr};
  callback_ = &invokeFunctionPointer<FuncPointer>;
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes>
template <typename Functor> inline
void FunctionReference<ReturnType (ArgumentTypes...)>::initFunctor(
    const Functor& functor) noexcept
{
  ::new (memory()) const void*{&functor};
  callback_ = &invokeFunctor<Functor>;
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes>
template <typename FuncPointer> inline
ReturnType FunctionReference<ReturnType (ArgumentTypes...)>::invokeFunctionPointer(
    const void* function_ptr,
    ArgumentTypes... arguments) noexcept
{
  const auto ptr = cast<const FuncPointer*>(function_ptr);
  return (*ptr)(static_cast<ArgumentTypes>(arguments)...);
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes>
template <typename Functor> inline
ReturnType FunctionReference<ReturnType (ArgumentTypes...)>::invokeFunctor(
    const void* functor,
    ArgumentTypes... arguments) noexcept
{
  const auto ptr = cast<const Functor* const*>(functor);
  return (*(*ptr))(static_cast<ArgumentTypes>(arguments)...);
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes> inline
void* FunctionReference<ReturnType (ArgumentTypes...)>::memory() noexcept
{
  return &memory_;
}

/*!
  */
template <typename ReturnType, typename ...ArgumentTypes> inline
const void* FunctionReference<ReturnType (ArgumentTypes...)>::memory() const noexcept
{
  return &memory_;
}

} // namespace zisc

#endif // ZISC_FUNCTION_REFERENCE_INL_HPP
