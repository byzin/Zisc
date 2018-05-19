/*!
  \file function_reference.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUNCTION_REFERENCE_HPP
#define ZISC_FUNCTION_REFERENCE_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "type_traits.hpp"
#include "utility.hpp"

namespace zisc {

template <typename> class FunctionReference;

/*!
  \brief A reference to a function
  */
template <typename ReturnType, typename ...ArgumentTypes>
class FunctionReference<ReturnType (ArgumentTypes...)>
{
 public:
  using FunctionPointer = ReturnType (*)(ArgumentTypes...);


  //! Create a reference to the function
  FunctionReference(FunctionPointer function_ptr) noexcept;

  //! Create a reference to the function
  template <typename RType, typename ...ArgTypes> 
  FunctionReference(
      RType (*function_ptr)(ArgTypes...),
      EnableIf<std::is_invocable_v<RType (*)(ArgTypes...),
                                   ArgumentTypes...>> = kEnabler) noexcept;

  //! Create a reference to the function
  template <typename Functor> 
  FunctionReference(
      const Functor& functor,
      EnableIf<std::is_invocable_v<Functor, ArgumentTypes...>> = kEnabler) noexcept;


  //! Invoke a referenced function
  template <typename ...ArgTypes>
  ReturnType operator()(ArgTypes&&... arguments) const noexcept;


  //! Invoke a referenced function
  template <typename ...ArgTypes>
  ReturnType invoke(ArgTypes&&... arguments) const noexcept;

 private:
  static constexpr std::size_t kStorageSize = zisc::max(
      sizeof(void*),
      sizeof(FunctionPointer));
  using Memory = std::aligned_union_t<kStorageSize, void*, FunctionPointer>;
  using CallbackPointer = ReturnType (*)(const void*, ArgumentTypes...) noexcept;


  //! Initialize a function reference
  template <typename FuncPointer>
  void initFunctionPointer(FuncPointer function_ptr) noexcept;

  //! Initialize a function reference
  template <typename Functor>
  void initFunctor(const Functor& functor) noexcept;

  //! Invoke a referenced function
  template <typename FuncPointer>
  static ReturnType invokeFunctionPointer(const void* function_ptr,
                                          ArgumentTypes... argments) noexcept;

  //! Invoke a referenced function
  template <typename Functor>
  static ReturnType invokeFunctor(const void* functor,
                                  ArgumentTypes... argments) noexcept;

  //! Return the memory of the function reference
  void* memory() noexcept;

  //! Return the memory of the function reference
  const void* memory() const noexcept;


  Memory memory_;
  CallbackPointer callback_;
};

} // namespace zisc

#include "function_reference-inl.hpp"

#endif // ZISC_FUNCTION_REFERENCE_HPP
