# file: compiler.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


#
function(initCompilerOptions)
  set(description "Enable compiler full warning (if supported).")
  setBooleanOption(Z_ENABLE_COMPILER_WARNING ON ${description})

  set(description "The compiler treat all warnings as build errors (if supported).")
  setBooleanOption(Z_TREAT_COMPILER_WARNING_AS_ERROR OFF ${description})

  set(description "Clang uses LLVM's build tools and libraries instead of platform specific tools.")
  setBooleanOption(Z_CLANG_USES_LLVM_TOOLS OFF ${description})

  set(description "Save intermediate compilation results.")
  setBooleanOption(Z_SAVE_INTERMEDIATE_COMPILATION_RESULTS OFF ${description})

  set(description "Enable some hardware features (Intel haswell or later, AMD zen or later).")
  setBooleanOption(Z_ENABLE_HARDWARE_FEATURES OFF ${description})

  set(description "Enable C++ address sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_SANITIZER_ADDRESS OFF ${description})

  set(description "Enable C++ thread sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_SANITIZER_THREAD OFF ${description})

  set(description "Enable C++ memory sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_SANITIZER_MEMORY OFF ${description})

  set(description "Enable C++ undefined sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR OFF ${description})

  set(description "Enable C++ undefined sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR_FULL OFF ${description})

  set(description "Enable C++ leak sanitizer (if compiler supports).")
  setBooleanOption(Z_ENABLE_SANITIZER_LEAK OFF ${description})

  set(description "Enable clang-tidy analyzer.")
  setBooleanOption(Z_ENABLE_STATIC_ANALYZER_CLANG_TIDY OFF ${description})

  set(description "Enable link-what-you-use analyzer.")
  setBooleanOption(Z_ENABLE_STATIC_ANALYZER_LWYU OFF ${description})
endfunction(initCompilerOptions)


function(getMsvcCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  list(APPEND compile_flags /constexpr:depth${constexpr_depth}
                            /constexpr:backtrace${constexpr_backtrace}
                            /constexpr:steps${constexpr_steps})

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(getMsvcCompilerFlags)


function(appendClangFlags cxx_compile_flags)
  set(compile_flags ${${cxx_compile_flags}})
  foreach(compile_flag IN LISTS ARGN)
    if(Z_VISUAL_STUDIO)
      string(REPLACE "=" ";" compile_flag ${compile_flag})
      foreach(flag IN LISTS compile_flag)
        list(APPEND compile_flags "SHELL:-Xclang ${flag}")
      endforeach(flag)
    else()
      list(APPEND compile_flags ${compile_flag})
    endif()
  endforeach(compile_flag)
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
endfunction(appendClangFlags)


function(getSanitizerFlags compile_sanitizer_flags linker_sanitizer_flags)
  set(check_list "")

  # Collect sanitizer checks
  if(Z_ENABLE_SANITIZER_ADDRESS)
    list(APPEND check_list address)
  endif()
  if(Z_ENABLE_SANITIZER_THREAD)
    list(APPEND check_list thread)
  endif()
  if(Z_ENABLE_SANITIZER_MEMORY)
    list(APPEND check_list memory)
  endif()
  if(Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR OR Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR_FULL)
    list(APPEND check_list alignment
                           bool
                           builtin
                           bounds
                           enum
                           float-cast-overflow
                           float-divide-by-zero
                           integer-divide-by-zero
                           nonnull-attribute
                           null
                           pointer-overflow
                           return
                           returns-nonnull-attribute
                           unreachable
                           vla-bound
                           vptr
                           )
    if(NOT Z_GCC)
      list(APPEND check_list nullability-arg
                             nullability-assign
                             nullability-return
                             function
                             )
    endif()
  endif()
  if(Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR_FULL)
    list(APPEND check_list shift
                           signed-integer-overflow
                           )
    if(NOT Z_GCC)
      list(APPEND check_list implicit-unsigned-integer-truncation
                             implicit-signed-integer-truncation
                             unsigned-integer-overflow
                             )
    endif()
  endif()
  if(Z_ENABLE_SANITIZER_LEAK)
    list(APPEND check_list leak)
  endif()

  # Make a sanitizer option string from check list 
  set(compile_flags "")
  set(linker_flags "")
  if(check_list)
    string(REPLACE ";" "," check_flag "${check_list}")
    set(compile_flags "-fsanitize=${check_flag}" -fno-omit-frame-pointer)
    set(linker_flags "-fsanitize=${check_flag}")

    # Output
    set(${compile_sanitizer_flags} ${compile_flags} PARENT_SCOPE)
    set(${linker_sanitizer_flags} ${linker_flags} PARENT_SCOPE)
  endif()
endfunction(getSanitizerFlags)


function(getClangCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  appendClangFlags(compile_flags
      -fconstexpr-depth=${constexpr_depth}
      -fconstexpr-backtrace-limit=${constexpr_backtrace}
      -fconstexpr-steps=${constexpr_steps}
      -ftemplate-depth=${recursive_template_depth})

  if(Z_ENABLE_HARDWARE_FEATURES)
    list(APPEND compile_flags -mfma)
  endif()

  if(Z_SAVE_INTERMEDIATE_COMPILATION_RESULTS)
    list(APPEND compile_flags -save-temps=obj)
  endif()

  if(Z_CLANG_USES_LLVM_TOOLS)
    list(APPEND compile_flags -stdlib=libc++)
    list(APPEND linker_flags -stdlib=libc++ -rtlib=compiler-rt)
    if(NOT Z_MAC)
      list(APPEND linker_flags -fuse-ld=lld)
    endif()
    list(APPEND definitions Z_CLANG_USES_LLVM_TOOLS=1)
  endif()

  # Sanitizer
  getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  list(APPEND compile_flags ${compile_sanitizer_flags})
  list(APPEND linker_flags ${linker_sanitizer_flags})

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(getClangCompilerFlags)


function(getGccCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  list(APPEND compile_flags -fconstexpr-depth=${constexpr_depth}
                            -fconstexpr-loop-limit=${constexpr_steps}
                            -ftemplate-depth=${recursive_template_depth})

  if(Z_ENABLE_HARDWARE_FEATURES)
    list(APPEND compile_flags -mfma)
  endif()

  if(Z_SAVE_INTERMEDIATE_COMPILATION_RESULTS)
    list(APPEND compile_flags -save-temps=obj)
  endif()

  # Sanitizer
  getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  list(APPEND compile_flags ${compile_sanitizer_flags})
  list(APPEND linker_flags ${linker_sanitizer_flags})

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(getGccCompilerFlags)


function(getMsvcWarningFlags compile_warning_flags)
  set(warning_flags "")
  list(APPEND warning_flags /W4)
  if(Z_TREAT_COMPILER_WARNING_AS_ERROR)
    list(APPEND warning_flags /WX)
  endif()
  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getMsvcWarningFlags)


function(getClangWarningFlags compile_warning_flags)
  set(warning_flags "")
  list(APPEND warning_flags -Weverything
                            -Wno-c++98-compat
                            -Wno-c++98-compat-pedantic
                            )
  if(Z_TREAT_COMPILER_WARNING_AS_ERROR)
    list(APPEND warning_flags -Werror)
  endif()
  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getClangWarningFlags)


function(getGccWarningFlags compile_warning_flags)
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
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getGccWarningFlags)


# Output functions
# Get compile options
function(getCxxCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)

  set(constexpr_depth 512)
  set(constexpr_backtrace 16)
  set(constexpr_steps 2147483647)
  set(recursive_template_depth 2048)

  if(Z_GCC)
    getGccCompilerFlags(compile_flags linker_flags definitions)
  elseif(Z_CLANG)
    getClangCompilerFlags(compile_flags linker_flags definitions)
  elseif(Z_MSVC)
    getMsvcCompilerFlags(compile_flags linker_flags definitions)
  endif()
  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(getCxxCompilerFlags)


#
function(getCxxWarningFlags compile_warning_flags)
  set(compiler_version ${CMAKE_CXX_COMPILER_VERSION})
  set(environment "${CMAKE_SYSTEM_NAME} ${CMAKE_CXX_COMPILER_ID} ${compiler_version}")

  set(warning_flags "")
  if(Z_ENABLE_COMPILER_WARNING)
    if(Z_GCC)
      getGccWarningFlags(warning_flags)
    elseif(Z_CLANG)
      getClangWarningFlags(warning_flags)
    elseif(Z_MSVC)
      getMsvcWarningFlags(warning_flags)
    else()
      message(WARNING "${environment}: Warning option isn't supported.")
    endif()
  endif()
  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getCxxWarningFlags)


#
function(setStaticAnalyzer target)
  set(static_analyzer_list "")

  # clang-tidy
  if(Z_ENABLE_STATIC_ANALYZER_CLANG_TIDY AND Z_CLANG)
    find_program(clang_tidy "clang-tidy")
    if(clang_tidy)
      set(tidy_command "${clang_tidy}")
      set(exception_check_list "")
      list(APPEND exception_check_list modernize-use-auto
                                       modernize-use-trailing-return-type
                                       modernize-avoid-c-arrays
                                       readability-uppercase-literal-suffix
                                       readability-magic-numbers
                                       readability-braces-around-statements
                                       readability-named-parameter
                                       readability-static-accessed-through-instance
                                       )
      set(exception_checks "")
      foreach(exception_check IN LISTS exception_check_list)
        set(exception_checks "${exception_checks},-${exception_check}")
      endforeach(exception_check)
      list(APPEND tidy_command "-checks=-*,clang-analyzer-*,modernize-*,performance-*,portability-*,readability-*${exception_checks}")
      set_target_properties(${target} PROPERTIES
          C_CLANG_TIDY "${tidy_command}"
          CXX_CLANG_TIDY "${tidy_command}")
      list(APPEND static_analyzer_list "clang-tidy")
    else()
      message(WARNING "[${target}] Could not find 'clang-tidy'.")
    endif()
  endif()

  # link-what-you-use
  if(Z_ENABLE_STATIC_ANALYZER_LWYU)
    set_target_properties(${target} PROPERTIES
        LINK_WHAT_YOU_USE TRUE)
    list(APPEND static_analyzer_list "link-what-you-use")
  endif()

  message(STATUS "[${target}] Static analyzer: ${static_analyzer_list}")
endfunction(setStaticAnalyzer)


function(getCxxFeatureList cxx_feature_list)
  set(feature_list "")
  list(APPEND feature_list
    cxx_aggregate_default_initializers
    cxx_alias_templates
    cxx_alignas
    cxx_alignof
    cxx_attributes
    cxx_attribute_deprecated
    cxx_auto_type
    cxx_binary_literals
    cxx_constexpr
    cxx_contextual_conversions
    # cxx_decltype_incomplete_return_types
    cxx_decltype
    cxx_decltype_auto
    cxx_default_function_template_args
    cxx_defaulted_functions
    cxx_defaulted_move_initializers
    cxx_delegating_constructors
    cxx_deleted_functions
    cxx_digit_separators
    cxx_enum_forward_declarations
    cxx_explicit_conversions
    cxx_extended_friend_declarations
    cxx_extern_templates
    cxx_final
    cxx_func_identifier
    cxx_generalized_initializers
    cxx_generic_lambdas
    cxx_inheriting_constructors
    cxx_inline_namespaces
    cxx_lambdas
    cxx_lambda_init_captures
    cxx_local_type_template_args
    cxx_long_long_type
    cxx_noexcept
    cxx_nonstatic_member_init
    cxx_nullptr
    cxx_override
    cxx_range_for
    cxx_raw_string_literals
    cxx_reference_qualified_functions
    cxx_relaxed_constexpr
    cxx_return_type_deduction
    cxx_right_angle_brackets
    cxx_rvalue_references
    cxx_sizeof_member
    cxx_static_assert
    cxx_strong_enums
    cxx_thread_local
    cxx_trailing_return_types
    cxx_unicode_literals
    cxx_uniform_initialization
    cxx_unrestricted_unions
    cxx_user_literals
    cxx_variable_templates
    cxx_variadic_macros
    cxx_variadic_templates
    cxx_template_template_parameters
    )
  set(${cxx_feature_list} ${feature_list} PARENT_SCOPE)
endfunction(getCxxFeatureList)


function(enableIpo target)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT output LANGUAGES CXX)
  if(ipo_result)
    set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    # message(WARNING "[${target}] IPO is not supported: ${output}")
    message(WARNING "[${target}] IPO is not supported.")
  endif()
endfunction(enableIpo)
