# file: compiler.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2018 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


#
function(initCompilerOption)
  set(option_description "Enable compiler full warning (if supported).")
  setBooleanOption(Z_ENABLE_COMPILER_WARNING ON ${option_description})

  set(option_description "The compiler treat all warnings as build errors (if supported).")
  setBooleanOption(Z_TREAT_COMPILER_WARNING_AS_ERROR OFF ${option_description})

  set(option_description "Clang uses libc++ instead of libstdc++ (if compiler supports).")
  setBooleanOption(Z_CLANG_USES_LIBCXX OFF ${option_description})

  set(option_description "Statically link C++ library")
  setBooleanOption(Z_STATIC_LINK_LIBCXX OFF ${option_description})

  set(option_description "Enable C++ address sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_ADDRESS_SANITIZER OFF ${option_description})

  set(option_description "Enable C++ leak sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_LEAK_SANITIZER OFF ${option_description})

  set(option_description "Enable C++ thread sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_THREAD_SANITIZER OFF ${option_description})

  set(option_description "Enable C++ undefined sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_UNDEFINED_SANITIZER OFF ${option_description})

  set(option_description "Enable C++ memory sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_MEMORY_SANITIZER OFF ${option_description})
endfunction(initCompilerOption)


function(getClangCompilerOption cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  if(Z_CLANG_USES_LIBCXX)
    list(APPEND compile_flags -stdlib=libc++)
    list(APPEND linker_flags -stdlib=libc++)
  endif()
  if(Z_STATIC_LINK_LIBCXX)
    list(APPEND linker_flags -static-libstdc++)
    if(Z_CLANG_USES_LIBCXX)
      list(APPEND linker_flags c++abi)
    endif()
  endif()
  if(Z_VISUAL_STUDIO)
    list(APPEND cxx_compile_flags /EHsc)
  endif()

  # Sanitizer
  set(sanitizer_flags "")
  if(Z_ENABLE_ADDRESS_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=address)
  endif()
  if(Z_ENABLE_LEAK_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=leak)
  endif()
  if(Z_ENABLE_THREAD_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=thread)
  endif()
  if(Z_ENABLE_UNDEFINED_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=undefined)
  endif()
  if(Z_ENABLE_MEMORY_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=memory)
  endif()
  if(Z_ENABLE_ADDRESS_SANITIZER OR Z_ENABLE_LEAK_SANITIZER OR
     Z_ENABLE_THREAD_SANITIZER OR Z_ENABLE_UNDEFINED_SANITIZER OR
     Z_ENABLE_MEMORY_SANITIZER)
    list(APPEND compile_flags ${sanitizer_flags} -fno-omit-frame-pointer)
    list(APPEND linker_flags ${sanitizer_flags})
  endif()

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(getClangCompilerOption)


function(getGccCompilerOption cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  if(Z_STATIC_LINK_LIBCXX)
    list(APPEND linker_flags -static-libstdc++)
  endif()

  # Sanitizer
  set(sanitizer_flags "")
  if(Z_ENABLE_ADDRESS_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=address)
  endif()
  if(Z_ENABLE_LEAK_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=leak)
  endif()
  if(Z_ENABLE_THREAD_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=thread)
  endif()
  if(Z_ENABLE_UNDEFINED_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=undefined)
  endif()
  if(Z_ENABLE_MEMORY_SANITIZER)
    list(APPEND sanitizer_flags -fsanitize=memory)
  endif()
  if(Z_ENABLE_ADDRESS_SANITIZER OR Z_ENABLE_LEAK_SANITIZER OR
     Z_ENABLE_THREAD_SANITIZER OR Z_ENABLE_UNDEFINED_SANITIZER OR
     Z_ENABLE_MEMORY_SANITIZER)
    list(APPEND compile_flags ${sanitizer_flags} -fno-omit-frame-pointer)
    list(APPEND linker_flags ${sanitizer_flags})
  endif()

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(getGccCompilerOption)


function(getClangWarningOption compiler_warning_flags)
  set(warning_flags "")
  if(Z_VISUAL_STUDIO)
    list(APPEND warning_flags /W4
                              )
    if(Z_TREAT_COMPILER_WARNING_AS_ERROR)
      list(APPEND warning_flags /WX)
    endif()
  else()
    list(APPEND warning_flags -Weverything
                              -Wno-c++98-compat
                              -Wno-c++98-compat-pedantic
                              )
    if(Z_TREAT_COMPILER_WARNING_AS_ERROR)
      list(APPEND warning_flags -Werror)
    endif()
  endif()
  # Output variables
  set(${compiler_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getClangWarningOption)


function(getGccWarningOption compiler_warning_flags)
  set(warning_flags "")
  list(APPEND warning_flags -pedantic
                            -Wall
                            -Wextra
                            -Wcast-align
                            -Wcast-qual
                            -Wctor-dtor-privacy
                            -Wdisabled-optimization
                            -Wformat=2
                            -Winit-self
                            -Wlogical-op
                            -Wmissing-declarations
                            -Wmissing-include-dirs
                            -Wnoexcept
                            -Wold-style-cast
                            -Woverloaded-virtual
                            -Wredundant-decls
                            -Wshadow
                            -Wsign-conversion
                            -Wsign-promo
                            -Wstrict-null-sentinel
                            -Wstrict-overflow=5
                            -Wswitch-default
                            -Wundef
                            )
  if(Z_TREAT_COMPILER_WARNING_AS_ERROR)
    list(APPEND warning_flags -Werror)
  endif()
  # Output variables
  set(${compiler_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getGccWarningOption)

function(getMsvcWarningOption compiler_warning_flags)
  set(warning_flags "")
  list(APPEND warning_flags /W4)
  if(Z_TREAT_COMPILER_WARNING_AS_ERROR)
    list(APPEND warning_flags /WX)
  endif()
  # Output variables
  set(${compiler_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getMsvcWarningOption)


# Output functions
# Get compile options
function(getCompilerOption cxx_compile_flags cxx_linker_flags cxx_definitions)
  if(Z_GCC)
    getGccCompilerOption(compile_flags linker_flags definitions)
  elseif(Z_CLANG)
    getClangCompilerOption(compile_flags linker_flags definitions)
  elseif(Z_MSVC)
    # Nothing
  endif()
  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(getCompilerOption)


#
function(getCxxWarningOption compiler_warning_flags)
  set(compiler_version ${CMAKE_CXX_COMPILER_VERSION})
  set(environment "${CMAKE_SYSTEM_NAME} ${CMAKE_CXX_COMPILER_ID} ${compiler_version}")

  set(warning_flags "")
  if(Z_ENABLE_COMPILER_WARNING)
    if(Z_GCC)
      getGccWarningOption(warning_flags)
    elseif(Z_CLANG)
      getClangWarningOption(warning_flags)
    elseif(Z_MSVC)
      getMsvcWarningOption(warning_flags)
    else()
      message(WARNING "${environment}: Warning option isn't supported.")
    endif()
  endif()
  # Output variables
  set(${compiler_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getCxxWarningOption)
