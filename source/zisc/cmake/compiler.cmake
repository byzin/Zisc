# file: compiler.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


#
function(Zisc_initCompilerOptions)
  include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake)

  set(description "Enable compiler full warning (if supported).")
  Zisc_setBooleanOption(Z_ENABLE_COMPILER_WARNING ON ${description})

  set(description "Make compiler warnings into errors (if supported).")
  Zisc_setBooleanOption(Z_MAKE_WARNING_INTO_ERROR OFF ${description})

  set(description "Clang uses LLVM's build tools and libraries instead of platform specific tools.")
  Zisc_setBooleanOption(Z_CLANG_USES_LLVM_TOOLS OFF ${description})

  set(description "Enable SIMD extensions (AVX2 and FMA on X64).")
  Zisc_setBooleanOption(Z_ENABLE_HARDWARE_FEATURE_SIMD OFF ${description})

  set(description "Enable C++ address sanitizer (if compiler supports).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_ADDRESS OFF ${description})

  set(description "Enable C++ thread sanitizer (if compiler supports).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_THREAD OFF ${description})

  set(description "Enable C++ memory sanitizer (if compiler supports).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_MEMORY OFF ${description})

  set(description "Enable C++ undefined sanitizer (if compiler supports).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR OFF ${description})

  set(description "Enable C++ undefined sanitizer (if compiler supports).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR_FULL OFF ${description})

  set(description "Enable C++ leak sanitizer (if compiler supports).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_LEAK OFF ${description})

  set(description "Enable clang-tidy analyzer.")
  Zisc_setBooleanOption(Z_ENABLE_STATIC_ANALYZER_CLANG_TIDY OFF ${description})

  set(description "Enable link-what-you-use analyzer.")
  Zisc_setBooleanOption(Z_ENABLE_STATIC_ANALYZER_LWYU OFF ${description})

  set(description "Save assembly files.")
  Zisc_setBooleanOption(Z_ENABLE_STATIC_ANALYZER_ASSEMBLY OFF ${description})
endfunction(Zisc_initCompilerOptions)


function(Zisc_appendClangDriverFlags cxx_compile_flags)
  set(compile_flags ${${cxx_compile_flags}})
  foreach(compile_flag IN LISTS ARGN)
    if(Z_VISUAL_STUDIO)
      set(compile_flag "/clang:${compile_flag}")
    endif()
    list(APPEND compile_flags ${compile_flag})
  endforeach(compile_flag)
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
endfunction(Zisc_appendClangDriverFlags)


function(Zisc_appendClangCompilerFlags cxx_compile_flags)
  set(compile_flags ${${cxx_compile_flags}})
  foreach(compile_flag IN LISTS ARGN)
    if(Z_VISUAL_STUDIO)
      string(REPLACE "=" ";" flags ${compile_flag})
      set(compile_flag "")
      foreach(flag IN LISTS flags)
        list(APPEND compile_flag "SHELL:-Xclang ${flag}")
      endforeach(flag)
    endif()
    list(APPEND compile_flags ${compile_flag})
  endforeach(compile_flag)
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
endfunction(Zisc_appendClangCompilerFlags)


function(Zisc_getSanitizerFlags compile_sanitizer_flags linker_sanitizer_flags)
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
    set(compile_flags "-fsanitize=${check_flag}")
    set(linker_flags "-fsanitize=${check_flag}")
    Zisc_appendClangDriverFlags(compile_flags -fno-omit-frame-pointer)

    # Output
    set(${compile_sanitizer_flags} ${compile_flags} PARENT_SCOPE)
    set(${linker_sanitizer_flags} ${linker_flags} PARENT_SCOPE)
  endif()
endfunction(Zisc_getSanitizerFlags)


function(Zisc_getMsvcCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  list(APPEND compile_flags /constexpr:depth${constexpr_depth}
                            /constexpr:backtrace${constexpr_backtrace}
                            /constexpr:steps${constexpr_steps})

  if(Z_ENABLE_HARDWARE_FEATURE_SIMD)
    list(APPEND compile_flags /arch:AVX2)
  endif()

  if(Z_ENABLE_STATIC_ANALYZER_ASSEMBLY)
    # list(APPEND compile_flags /FA)
  endif()

  # Optimization

  # Diagnostic
  list(APPEND compile_flags /diagnostics:caret
                            )
  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(Zisc_getMsvcCompilerFlags)


function(Zisc_getClangClCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  Zisc_appendClangCompilerFlags(compile_flags
      -fconstexpr-depth=${constexpr_depth}
      -fconstexpr-backtrace-limit=${constexpr_backtrace}
      -fconstexpr-steps=${constexpr_steps}
      -ftemplate-depth=${recursive_template_depth})

  if(Z_ENABLE_HARDWARE_FEATURE_SIMD)
    list(APPEND compile_flags /clang:-fno-math-errno)
    list(APPEND compile_flags /arch:AVX2)
  endif()

  if(Z_ENABLE_STATIC_ANALYZER_ASSEMBLY)
    list(APPEND compile_flags /FA)
  endif()

  # Optimization
  list(APPEND compile_flags /Qvec # Auto loop-vectorization
                            )

  # Diagnostic
  list(APPEND compile_flags /diagnostics:caret
                            -fcolor-diagnostics
                            )
  # Sanitizer
  Zisc_getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  list(APPEND compile_flags ${compile_sanitizer_flags})
  list(APPEND linker_flags ${linker_sanitizer_flags})

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(Zisc_getClangClCompilerFlags)


function(Zisc_getClangCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  list(APPEND compile_flags -fconstexpr-depth=${constexpr_depth}
                            -fconstexpr-backtrace-limit=${constexpr_backtrace}
                            -fconstexpr-steps=${constexpr_steps}
                            -ftemplate-depth=${recursive_template_depth})

  if(Z_ENABLE_HARDWARE_FEATURE_SIMD)
    list(APPEND compile_flags -fno-math-errno -mavx2 -mfma)
  endif()

  if(Z_ENABLE_STATIC_ANALYZER_ASSEMBLY)
    list(APPEND compile_flags -save-temps=obj
                              -fverbose-asm)
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
  Zisc_getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  list(APPEND compile_flags ${compile_sanitizer_flags})
  list(APPEND linker_flags ${linker_sanitizer_flags})

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(Zisc_getClangCompilerFlags)


function(Zisc_getGccCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  list(APPEND compile_flags -fconstexpr-depth=${constexpr_depth}
                            -fconstexpr-loop-limit=${constexpr_steps}
                            -ftemplate-depth=${recursive_template_depth})

  if(Z_ENABLE_HARDWARE_FEATURE_SIMD)
    list(APPEND compile_flags -mavx2 -mfma)
  endif()

  if(Z_ENABLE_STATIC_ANALYZER_ASSEMBLY)
    list(APPEND compile_flags -save-temps=obj
                              -fverbose-asm)
  endif()

  # Sanitizer
  Zisc_getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  list(APPEND compile_flags ${compile_sanitizer_flags})
  list(APPEND linker_flags ${linker_sanitizer_flags})

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(Zisc_getGccCompilerFlags)


function(Zisc_getMsvcWarningFlags compile_warning_flags)
  set(warning_flags "")
  list(APPEND warning_flags /W4)
  if(Z_MAKE_WARNING_INTO_ERROR)
    list(APPEND warning_flags /WX)
  endif()
  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getMsvcWarningFlags)


function(Zisc_getClangClWarningFlags compile_warning_flags)
  set(warning_flags "")
  list(APPEND warning_flags /Wall
                            -Wno-c++98-compat
                            -Wno-c++98-compat-pedantic
                            )
  if(Z_MAKE_WARNING_INTO_ERROR)
    list(APPEND warning_flags /WX)
  endif()
  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getClangClWarningFlags)


function(Zisc_getClangWarningFlags compile_warning_flags)
  set(warning_flags "")
  list(APPEND warning_flags -Weverything
                            -Wno-c++98-compat
                            -Wno-c++98-compat-pedantic
                            )
  if(Z_MAKE_WARNING_INTO_ERROR)
    list(APPEND warning_flags -Werror)
  endif()
  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getClangWarningFlags)


function(Zisc_getGccWarningFlags compile_warning_flags)
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
  if(Z_MAKE_WARNING_INTO_ERROR)
    list(APPEND warning_flags -Werror)
  endif()
  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getGccWarningFlags)


# Output functions
# Get compile options
function(Zisc_getCxxCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)

  set(constexpr_depth 512)
  set(constexpr_backtrace 16)
  set(constexpr_steps 2147483647)
  set(recursive_template_depth 2048)

  if(Z_GCC)
    Zisc_getGccCompilerFlags(compile_flags linker_flags definitions)
  elseif(Z_CLANG)
    if(Z_VISUAL_STUDIO)
      Zisc_getClangClCompilerFlags(compile_flags linker_flags definitions)
    else()
      Zisc_getClangCompilerFlags(compile_flags linker_flags definitions)
    endif()
  elseif(Z_MSVC)
    Zisc_getMsvcCompilerFlags(compile_flags linker_flags definitions)
  endif()
  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(Zisc_getCxxCompilerFlags)


#
function(Zisc_getCxxWarningFlags compile_warning_flags)
  set(compiler_version ${CMAKE_CXX_COMPILER_VERSION})
  set(environment "${CMAKE_SYSTEM_NAME} ${CMAKE_CXX_COMPILER_ID} ${compiler_version}")

  set(warning_flags "")
  if(Z_ENABLE_COMPILER_WARNING)
    if(Z_GCC)
      Zisc_getGccWarningFlags(warning_flags)
    elseif(Z_CLANG)
      if(Z_VISUAL_STUDIO)
        Zisc_getClangClWarningFlags(warning_flags)
      else()
        Zisc_getClangWarningFlags(warning_flags)
      endif()
    elseif(Z_MSVC)
      Zisc_getMsvcWarningFlags(warning_flags)
    else()
      message(WARNING "${environment}: Warning option isn't supported.")
    endif()
  endif()
  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getCxxWarningFlags)


#
function(Zisc_setStaticAnalyzer target)
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

  # Assembly
  if(Z_ENABLE_STATIC_ANALYZER_ASSEMBLY)
    get_target_property(binary_dir ${target} BINARY_DIR)
    set(assembly_target ${target}_assembly)

    # Generate a script
    set(assembly_dir ${binary_dir}/assembly/${target})
    set(script "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n"
               "file(MAKE_DIRECTORY \"${assembly_dir}\")\n")
    if(Z_CLANG)
      # Search bitcode files
      string(APPEND script
          "Zisc_createLinkToTargetTempFiles(\"${target}\" \"${binary_dir}\" \"${assembly_dir}\" bc)\n")
    endif()
    if(Z_LINUX OR Z_MAC)
      # Search assembly files
      string(APPEND script
          "Zisc_createLinkToTargetTempFiles(\"${target}\" \"${binary_dir}\" \"${assembly_dir}\" s)\n")
    endif()
    if(Z_WINDOWS)
      # Search COFF files
      string(APPEND script
          "Zisc_createLinkToTargetTempFiles(\"${target}\" \"${binary_dir}\" \"${assembly_dir}\" obj asm)\n")
    endif()

    # Save the script
    set(script_dir ${binary_dir}/scripts)
    file(MAKE_DIRECTORY ${script_dir})
    set(script_file ${script_dir}/${assembly_target}.cmake)
    file(WRITE ${script_file} ${script})

    add_custom_target(
        ${assembly_target} ALL
        ${CMAKE_COMMAND} -P ${script_file}
        DEPENDS ${target}
        WORKING_DIRECTORY "${binary_dir}"
        COMMENT "Create links to the assembly of '${target}' into '${assembly_dir}'."
        SOURCES ${script_file})

    list(APPEND static_analyzer_list "assembly")
  endif()

  message(STATUS "[${target}] Static analyzer: ${static_analyzer_list}")
endfunction(Zisc_setStaticAnalyzer)


function(Zisc_createLinkToFiles output_dir)
  foreach(file IN LISTS ARGN)
    get_filename_component(name ${file} NAME)
    file(CREATE_LINK ${file} "${output_dir}/${name}" RESULT result)
    if(result)
      message("${result}")
    endif()
  endforeach(file)
endfunction(Zisc_createLinkToFiles)


function(Zisc_createLinkToTarget target output_dir)
  get_target_property(binary_dir ${target} BINARY_DIR)
  set(link_target ${target}_link)

  set(script
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n"
      "get_filename_component(target_name \"\${target_path}\" NAME)\n"
      "if(NOT EXISTS \"${output_dir}/\${target_name}\")\n"
      "  Zisc_createLinkToFiles(\"${output_dir}\" \"\${target_path}\")\n"
      "endif()\n")
  set(script_dir ${binary_dir}/scripts)
  file(MAKE_DIRECTORY ${script_dir})
  set(script_file ${script_dir}/${link_target}.cmake)
  file(WRITE ${script_file} ${script})

  add_custom_target(
      ${link_target} ALL
      ${CMAKE_COMMAND} -D target_path=$<TARGET_FILE:${target}> -P ${script_file}
      DEPENDS ${target}
      WORKING_DIRECTORY "${binary_dir}"
      COMMENT "Create a link to the target '${target}' into '${output_dir}'"
      SOURCE ${script_file})
endfunction(Zisc_createLinkToTarget)


function(Zisc_createLinkToTargetTempFiles target binary_dir output_dir)
  find_file(tmp_dir
      "${target}.dir"
      PATHS ${binary_dir}
      PATH_SUFFIXES CMakeFiles
      DOC "Target '${target}' temporal directory"
      NO_DEFAULT_PATH)
  if(tmp_dir)
    foreach(type IN LISTS ARGN)
      file(GLOB_RECURSE files LIST_DIRECTORIES false "${tmp_dir}/*.${type}")
      Zisc_createLinkToFiles("${output_dir}" ${files})
    endforeach(type)
  else()
    message(WARNING "'${target}.dir' not found.")
  endif()
endfunction(Zisc_createLinkToTargetTempFiles)


function(Zisc_getCxxFeatureList cxx_feature_list)
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
endfunction(Zisc_getCxxFeatureList)


function(Zisc_enableIpo target)
  if(Z_VISUAL_STUDIO AND Z_CLANG)
    target_compile_options(${target} PRIVATE -flto=thin)
    message(STATUS "[${target}] Enable IPO/LTO.")
    return()
  endif()

  set(result OFF)
  if(Z_LINUX AND Z_CLANG AND Z_CLANG_USES_LLVM_TOOLS)
    set(result ON)
  else()
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output LANGUAGES CXX)
  endif()

  if(result)
    message(STATUS "[${target}] Enable IPO/LTO.")
    set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    # message(STATUS "[${target}] IPO is not supported: ${output}")
    message(STATUS "[${target}] IPO/LTO isn't supported.")
  endif()
endfunction(Zisc_enableIpo)
