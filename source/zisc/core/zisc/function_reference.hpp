/*!
  \file function_reference.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
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
  using ReturnType = ReturnT;
  using FunctionPointer = ReturnType (*)(ArgTypes...);


  //! Create an empry
  FunctionReference() noexcept;

  //! Create a reference to the given callable object
  template <InvocableR<ReturnT, ArgTypes...> Func>
  FunctionReference(Func&& func) noexcept;


  //! Invoke a referenced callable object
  template <typename ...Args>
  ReturnType operator()(Args&&... args) const;

  //! Check whether this refers a callable object 
  explicit operator bool() const noexcept;


  //! Create a reference to the given callable object
  template <InvocableR<ReturnT, ArgTypes...> Func>
  FunctionReference& assign(Func&& func) noexcept;

  //! Clear the underlying reference to a callable object
  void clear() noexcept;

  //! Invoke a referenced callable object
  template <typename ...Args>
  ReturnType invoke(Args&&... args) const;

  //! Exchange referenced callable objects of this and other
  void swap(FunctionReference& other) noexcept;


  static constexpr std::size_t kNumOfArgs = sizeof...(ArgTypes);

 private:
  static constexpr std::size_t kStorageSize = (std::max)(sizeof(void*),
                                                         sizeof(FunctionPointer));
  using Memory = std::aligned_union_t<kStorageSize, void*, FunctionPointer>;
  using CallbackPointer = ReturnType (*)(const void*, ArgTypes...);


  //! Initialize with a callable object
  template <InvocableR<ReturnT, ArgTypes...> Func>
  void initialize(Func&& func) noexcept;

  //! Invoke a referenced callable object 
  template <typename FuncPointer>
  static ReturnType invokeFunctionPointer(const void* func_ptr, ArgTypes... args);

  //! Invoke a referenced callable object
  template <typename Functor>
  static ReturnType invokeFunctor(const void* func_ptr, ArgTypes... args);

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
