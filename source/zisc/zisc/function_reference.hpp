/*!
  \file function_reference.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUNCTION_REFERENCE_HPP
#define ZISC_FUNCTION_REFERENCE_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "algorithm.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace zisc {

template <typename> class FunctionReference;

/*!
  \brief A reference to a function

  No detailed description.

  \tparam ReturnT No description.
  \tparam ArgTypes No description.
  */
template <typename ReturnT, typename ...ArgTypes>
class FunctionReference<ReturnT (ArgTypes...)>
{
 public:
  using ReturnType = ReturnT;
  using FunctionPointer = ReturnType (*)(ArgTypes...);


  template <typename Function>
  static constexpr bool kIsInvocableRaw =
      std::is_invocable_v<Function, ArgTypes...> &&
      !std::is_same_v<FunctionReference,
                      std::remove_cv_t<std::remove_reference_t<Function>>>;


  //! Create an empry
  FunctionReference() noexcept;

  //! Create a reference to a function
  template <typename Function>
  FunctionReference(Function&& func,
                    EnableIf<kIsInvocableRaw<Function>> = kEnabler) noexcept;


  //! Invoke a referenced callable object
  template <typename ...Args>
  ReturnType operator()(Args&&... arguments) const;

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
  template <typename ...Args>
  ReturnType invoke(Args&&... arguments) const;

  //! Exchange referenced callable objects of this and other
  void swap(FunctionReference& other) noexcept;


  static constexpr std::size_t kNumOfArgs = sizeof...(ArgTypes);

 private:
  static constexpr std::size_t kStorageSize = zisc::max(
      sizeof(void*),
      sizeof(FunctionPointer));
  using Memory = std::aligned_union_t<kStorageSize, void*, FunctionPointer>;
  using CallbackPointer = ReturnType (*)(const void*, ArgTypes...);


  //! Initialize with a function
  template <typename Function>
  void initialize(Function&& func) noexcept;

  //! Invoke a referenced callable object 
  template <typename FuncPointer>
  static ReturnType invokeFunctionPointer(const void* function_ptr,
                                          ArgTypes... argments);

  //! Invoke a referenced callable object
  template <typename Functor>
  static ReturnType invokeFunctor(const void* function_memory,
                                  ArgTypes... argments);

  //! Return the memory of the function reference
  void* memory() noexcept;

  //! Return the memory of the function reference
  const void* memory() const noexcept;


  Memory memory_;
  CallbackPointer callback_ = nullptr;
};

//! Swap memories in the given instances
template <typename ReturnT, typename ...ArgTypes>
void swap(FunctionReference<ReturnT (ArgTypes...)>& lhs,
          FunctionReference<ReturnT (ArgTypes...)>& rhs) noexcept;

} // namespace zisc

#include "function_reference-inl.hpp"

#endif // ZISC_FUNCTION_REFERENCE_HPP
