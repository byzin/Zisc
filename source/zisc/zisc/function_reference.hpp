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
 public:
  using ReturnType = ReturnT;
  using FunctionPointer = ReturnType (*)(ArgumentTypes...);


  template <typename Function>
  static constexpr bool kIsInvocableRaw =
      std::is_invocable_v<Function, ArgumentTypes...> &&
      !std::is_same_v<FunctionReference,
                      std::remove_cv_t<std::remove_reference_t<Function>>>;


  //! Create an empry
  FunctionReference() noexcept;

  //! Create a reference to a function
  template <typename Function>
  FunctionReference(
      Function&& func,
      EnableIf<kIsInvocableRaw<Function>> = kEnabler) noexcept;


  //! Invoke a referenced callable object
  template <typename ...ArgTypes>
  ReturnType operator()(ArgTypes&&... arguments) const noexcept;

  //! Check whether this refers a callable object 
  explicit operator bool() const noexcept
  {
    return callback_ != nullptr;
  }


  //! Copy a function reference
  FunctionReference& assign(const FunctionReference& other) noexcept;

  //! Refer a function pointer
  template <typename Function>
  FunctionReference& assign(
      Function&& func,
      EnableIf<kIsInvocableRaw<Function>> = kEnabler) noexcept;

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


  //! Initialize with a function
  template <typename Function>
  void initialize(Function&& func) noexcept;

  //! Invoke a referenced callable object 
  template <typename FuncPointer>
  static ReturnType invokeFunctionPointer(const void* function_ptr,
                                          ArgumentTypes... argments) noexcept;

  //! Invoke a referenced callable object
  template <typename Functor>
  static ReturnType invokeFunctor(const void* function_memory,
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
