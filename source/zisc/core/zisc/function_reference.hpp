/*!
  \file function_reference.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUNCTION_REFERENCE_HPP
#define ZISC_FUNCTION_REFERENCE_HPP

// Standard C++ library
#include <algorithm>
#include <cstddef>
#include <type_traits>
// Zisc
#include "concepts.hpp"

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
  // Type aliases
  using FunctionPointer = ReturnT (*)(ArgTypes...);


  // Constant value
  static constexpr std::size_t kNumOfArgs = sizeof...(ArgTypes);


  //! Create an empry
  FunctionReference() noexcept = default;

  //! Create a reference to the given callable object
  template <InvocableR<ReturnT, ArgTypes...> Func>
  FunctionReference(Func&& func) noexcept;


  //! Create a reference to the given callable object
  template <InvocableR<ReturnT, ArgTypes...> Func>
  FunctionReference& operator=(Func&& func) noexcept;

  //! Invoke a referenced callable object
  ReturnT operator()(ArgTypes... args) const;

  //! Check whether this refers a callable object 
  explicit operator bool() const noexcept;


  //! Create a reference to the given callable object
  template <InvocableR<ReturnT, ArgTypes...> Func>
  FunctionReference& assign(Func&& func) noexcept;

  //! Clear the underlying reference to a callable object
  void clear() noexcept;

  //! Invoke a referenced callable object
  ReturnT invoke(ArgTypes... args) const;

  //! Exchange referenced callable objects of this and other
  void swap(FunctionReference& other) noexcept;

 private:
  // Type aliases
  using FuncRefMemory = std::aligned_storage_t<sizeof(void*),
                                               std::alignment_of_v<void*>>;
  using InvokerPointer = ReturnT (*)(FuncRefMemory, ArgTypes...);
  template <typename Type>
  using ArgRef = std::add_lvalue_reference_t<std::remove_reference_t<Type>>;


  //! Forward lvalue as either lvalue or as rvalue
  template <typename Type>
  static constexpr Type forward(ArgRef<Type> arg) noexcept;

  //! Initialize with a callable object
  template <InvocableR<ReturnT, ArgTypes...> Func>
  void initialize(Func&& func) noexcept;

  //! Invoke a referenced callable object 
  template <typename FuncPtr>
  static ReturnT invokeFunctionPointer(FuncRefMemory mem, ArgTypes... args);

  //! Invoke a referenced callable object
  template <typename Functor>
  static ReturnT invokeFunctor(FuncRefMemory mem, ArgTypes... args);

  //! Return the underlying invoker pointer
  const InvokerPointer& invoker() const noexcept;

  //! Return the memory of the function reference
  FuncRefMemory& memory() noexcept;

  //! Return the memory of the function reference
  const FuncRefMemory& memory() const noexcept;


  FuncRefMemory memory_;
  InvokerPointer invoker_ = nullptr;
};

//! Swap memories in the given instances
template <typename ReturnT, typename ...ArgTypes>
void swap(FunctionReference<ReturnT (ArgTypes...)>& lhs,
          FunctionReference<ReturnT (ArgTypes...)>& rhs) noexcept;

} // namespace zisc

#include "function_reference-inl.hpp"

#endif // ZISC_FUNCTION_REFERENCE_HPP
