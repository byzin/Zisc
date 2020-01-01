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
#include <type_traits>
// Zisc
#include "error.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
FunctionReference<ReturnT (ArgumentTypes...)>::FunctionReference() noexcept
{
}

/*!
  \details No detailed description

  \tparam Function No description.
  \param [in] func No description.
  */
template <typename ReturnT, typename ...ArgumentTypes>
template <typename Function> inline
FunctionReference<ReturnT (ArgumentTypes...)>::FunctionReference(
    Function&& func,
    EnableIf<kIsInvocableRaw<Function>>) noexcept
{
  initialize<Function>(std::forward<Function>(func));
}

/*!
  \details No detailed description

  \tparam ArgTypes No description.
  \param [in] arguments No description.
  \return No description
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
  \details No detailed description

  \param [in] other No description.
  \return No description
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
  \details No detailed description

  \tparam Function No description.
  \param [in] func No description.
  \return No description
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
  \details No detailed description
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
void FunctionReference<ReturnT (ArgumentTypes...)>::clear() noexcept
{
  callback_ = nullptr;
}

/*!
  \details No detailed description

  \tparam ArgTypes No description.
  \param [in] arguments No description.
  \return No description
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
  \details No detailed description

  \param [in,out] other No description.
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
void FunctionReference<ReturnT (ArgumentTypes...)>::swap(
    FunctionReference& other) noexcept
{
  zisc::swap(memory_, other.memory_);
  zisc::swap(callback_, other.callback_);
}

/*!
  \details No detailed description

  \tparam Function No description.
  \param [in] func No description.
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
  \details No detailed description

  \tparam FuncPointer No description.
  \param [in] function_ptr No description.
  \param [in] arguments No description.
  \return No description
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
  \details No detailed description

  \tparam Functor No description.
  \param [in] functor No description.
  \param [in] arguments No description.
  \return No description
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
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
void* FunctionReference<ReturnT (ArgumentTypes...)>::memory() noexcept
{
  return &memory_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
const void* FunctionReference<ReturnT (ArgumentTypes...)>::memory() const noexcept
{
  return &memory_;
}

/*!
  \details No detailed description

  \tparam ReturnT No description.
  \tparam ArgumentTypes No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  */
template <typename ReturnT, typename ...ArgumentTypes> inline
void swap(FunctionReference<ReturnT (ArgumentTypes...)>& lhs,
          FunctionReference<ReturnT (ArgumentTypes...)>& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace zisc

#endif // ZISC_FUNCTION_REFERENCE_INL_HPP
