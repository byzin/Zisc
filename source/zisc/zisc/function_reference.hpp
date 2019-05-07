/*!
  \file function_reference.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
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
template <typename ReturnT, typename ...ArgumentTypes>
class FunctionReference<ReturnT (ArgumentTypes...)>
{
//! \todo Unify the invocable function
#if defined(Z_MAC) && defined(Z_CLANG)
  template <class Function, class ...ArgTypes>
  static constexpr bool is_invocable_v = std::__invokable<Function, ArgTypes...>::value;
#else
  template <class Function, class ...ArgTypes>
  static constexpr bool is_invocable_v = std::is_invocable_v<Function, ArgTypes...>;
#endif

 public:
  using ReturnType = ReturnT;
  using FunctionPointer = ReturnType (*)(ArgumentTypes...);


  //! Create an empry
  FunctionReference() noexcept;

  //! Create a reference to a function pointer
  FunctionReference(FunctionPointer function_ptr) noexcept;

  //! Create a reference to a function pointer
  template <typename RType, typename ...ArgTypes> 
  FunctionReference(
      RType (*function_ptr)(ArgTypes...),
      EnableIf<is_invocable_v<RType (*)(ArgTypes...), ArgumentTypes...>> = kEnabler)
          noexcept;

  //! Create a reference to a functor 
  template <typename Functor> 
  FunctionReference(
      const Functor& functor,
      EnableIf<is_invocable_v<Functor, ArgumentTypes...>> = kEnabler) noexcept;


  //! Invoke a referenced callable object
  template <typename ...ArgTypes>
  ReturnType operator()(ArgTypes&&... arguments) const noexcept;

  //! Check whether this refers a callable object 
  explicit operator bool() const noexcept
  {
    return callback_ != nullptr;
  }


  //! Refer a function pointer
  FunctionReference& assign(FunctionPointer function_ptr) noexcept;

  //! Refer a function pointer
  template <typename RType, typename ...ArgTypes> 
  FunctionReference& assign(
      RType (*function_ptr)(ArgTypes...),
      EnableIf<is_invocable_v<RType (*)(ArgTypes...), ArgumentTypes...>> = kEnabler)
          noexcept;

  //! Refer a functor
  template <typename Functor> 
  FunctionReference& assign(
      const Functor& functor,
      EnableIf<is_invocable_v<Functor, ArgumentTypes...>> = kEnabler) noexcept;

  //! Clear the stored callable object
  void clear() noexcept;

  //! Invoke a referenced callable object
  template <typename ...ArgTypes>
  ReturnType invoke(ArgTypes&&... arguments) const noexcept;

  //! Exchange referenced callable objects of this and other
  void swap(FunctionReference& other) noexcept;


  static constexpr std::size_t kNumOfArgs = sizeof...(ArgumentTypes);

 private:
  static constexpr std::size_t kStorageSize = zisc::max(
      sizeof(void*),
      sizeof(FunctionPointer));
  using Memory = std::aligned_union_t<kStorageSize, void*, FunctionPointer>;
  using CallbackPointer = ReturnType (*)(const void*, ArgumentTypes...) noexcept;


  //! Initialize with a function pointer
  template <typename FuncPointer>
  void initFunctionPointer(FuncPointer function_ptr) noexcept;

  //! Initialize with a functor
  template <typename Functor>
  void initFunctor(const Functor& functor) noexcept;

  //! Invoke a referenced callable object 
  template <typename FuncPointer>
  static ReturnType invokeFunctionPointer(const void* function_ptr,
                                          ArgumentTypes... argments) noexcept;

  //! Invoke a referenced callable object
  template <typename Functor>
  static ReturnType invokeFunctor(const void* functor,
                                  ArgumentTypes... argments) noexcept;

  //! Return the memory of the function reference
  void* memory() noexcept;

  //! Return the memory of the function reference
  const void* memory() const noexcept;


  Memory memory_;
  CallbackPointer callback_ = nullptr;
};

template <typename ReturnT, typename ...ArgumentTypes>
void swap(FunctionReference<ReturnT (ArgumentTypes...)>& lhs,
          FunctionReference<ReturnT (ArgumentTypes...)>& rhs) noexcept;

} // namespace zisc

#include "function_reference-inl.hpp"

#endif // ZISC_FUNCTION_REFERENCE_HPP
