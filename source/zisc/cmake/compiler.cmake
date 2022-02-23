# file: compiler.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2022 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


#
function(Zisc_initCompilerOptions)
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake")

  set(description "Enable compiler recommended warnings.")
  Zisc_setBooleanOption(Z_ENABLE_COMPILER_WARNING ON "${description}")

  set(description "Enable compiler full warnings (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_COMPILER_WARNING_EXTRA OFF "${description}")

  set(description "Make compiler warnings into errors (if supported that).")
  Zisc_setBooleanOption(Z_MAKE_WARNING_INTO_ERROR OFF "${description}")

  set(description "Clang uses LLVM's build tools and libraries instead of platform specific tools.")
  Zisc_setBooleanOption(Z_CLANG_USES_LLVM_TOOLS OFF "${description}")

  set(description "Enable recent hardware features (e.g. SIMD, half precision floating point). Assume 'x86-64-v3' is supported on x86-64.")
  Zisc_setBooleanOption(Z_ENABLE_RECENT_HARDWARE_FEATURES OFF "${description}")

  set(description "Enable C++ address sanitizer (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_ADDRESS OFF "${description}")

  set(description "Enable C++ thread sanitizer (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_THREAD OFF "${description}")

  set(description "Enable C++ memory sanitizer (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_MEMORY OFF "${description}")

  set(description "Enable C++ undefined sanitizer (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR OFF "${description}")

  set(description "Enable C++ undefined sanitizer (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR_FULL OFF "${description}")

  set(description "Enable C++ leak sanitizer (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_LEAK OFF "${description}")

  # TODO. lto=full is required
  #  set(description "Enable C++ control flow integrity (if compiler supports that).")
  #  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_CFI OFF "${description}")

  set(description "Enable C++ safe stack protection (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_SANITIZER_SAFE_STACK OFF "${description}")

  set(description "Enable clang-tidy analyzer.")
  Zisc_setBooleanOption(Z_ENABLE_STATIC_ANALYZER_CLANG_TIDY OFF "${description}")

  set(description "Enable link-what-you-use analyzer.")
  Zisc_setBooleanOption(Z_ENABLE_STATIC_ANALYZER_LWYU OFF "${description}")

  set(description "Save files show the hint of optimization.")
  Zisc_setBooleanOption(Z_ENABLE_STATIC_ANALYZER_OPTIMIZATION OFF "${description}")
endfunction(Zisc_initCompilerOptions)


function(Zisc_getSanitizerFlags compile_sanitizer_flags linker_sanitizer_flags)
  set(compile_flags "")
  set(linker_flags "")
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
                           object-size
                           pointer-overflow
                           return
                           returns-nonnull-attribute
                           unreachable
                           vla-bound
                           vptr
                           )
    if(NOT Z_GCC)
      list(APPEND check_list function
                             nullability-arg
                             nullability-assign
                             nullability-return
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
                             unsigned-shift-base
                             unsigned-integer-overflow
                             )
      if(NOT Z_APPLE_CLANG)
        list(APPEND check_list implicit-integer-sign-change
                               )
      endif()
    endif()
  endif()
  if(Z_ENABLE_SANITIZER_LEAK)
    list(APPEND check_list leak)
  endif()
  if(Z_ENABLE_SANITIZER_CFI)
    list(APPEND check_list cfi)
    list(APPEND compile_flags -fno-sanitize-trap)
  endif()
  if(Z_ENABLE_SANITIZER_SAFE_STACK)
    list(APPEND check_list safe-stack)
  endif()

  # Make a sanitizer option string from check list 
  if(check_list)
    string(REPLACE ";" "," check_flag "${check_list}")
    if(Z_VISUAL_STUDIO)
      list(APPEND compile_flags "/fsanitize=${check_flag}")
    else()
      list(APPEND compile_flags "-fsanitize=${check_flag}")
      list(APPEND compile_flags -fno-omit-frame-pointer)
      list(APPEND linker_flags "-fsanitize=${check_flag}")
    endif()

    # Output
    set(${compile_sanitizer_flags} ${compile_flags} PARENT_SCOPE)
    set(${linker_sanitizer_flags} ${linker_flags} PARENT_SCOPE)
  endif()
endfunction(Zisc_getSanitizerFlags)


function(Zisc_getMsvcCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  # Optimization
  list(APPEND compile_flags /favor:AMD64)
  if(Z_ENABLE_RECENT_HARDWARE_FEATURES)
    list(APPEND compile_flags /arch:AVX2)
  endif()

  # Diagnostic
  list(APPEND compile_flags /diagnostics:caret
                            /nologo
                            /fastfail
                            /options:strict
                            )

  # Security
  list(APPEND compile_flags # /Qspectre
                            /sdl
                            )

  # Sanitizer
  Zisc_getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  list(APPEND compile_flags ${compile_sanitizer_flags})
  list(APPEND linker_flags ${linker_sanitizer_flags})

  # Output variables
  set(${cxx_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${cxx_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${cxx_definitions} ${definitions} PARENT_SCOPE)
endfunction(Zisc_getMsvcCompilerFlags)


function(Zisc_getClangClCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  # Optimization
  if(Z_ENABLE_RECENT_HARDWARE_FEATURES)
    list(APPEND compile_flags /clang:-fno-math-errno)
    list(APPEND compile_flags /clang:-march=x86-64-v3)
  endif()
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

  if(Z_ENABLE_STATIC_ANALYZER_OPTIMIZATION)
    list(APPEND compile_flags -save-temps=obj
                              -fverbose-asm)
  endif()

  # Optimization
  if(Z_ENABLE_RECENT_HARDWARE_FEATURES)
    list(APPEND compile_flags -fno-math-errno
                              -march=x86-64-v3
                              )
  endif()

  if(Z_CLANG_USES_LLVM_TOOLS)
    list(APPEND compile_flags -stdlib=libc++)
    list(APPEND linker_flags -stdlib=libc++ -rtlib=compiler-rt)
    if(NOT Z_APPLE_CLANG)
      list(APPEND linker_flags -fuse-ld=lld)
    endif()
    list(APPEND definitions Z_CLANG_USES_LLVM_TOOLS=1)
  endif()

  # Diagnostic
  list(APPEND compile_flags -fcolor-diagnostics
                            )

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

  if(Z_ENABLE_STATIC_ANALYZER_OPTIMIZATION)
    list(APPEND compile_flags -save-temps=obj
                              -fverbose-asm)
  endif()

  # Optimization
  if(Z_ENABLE_RECENT_HARDWARE_FEATURES)
    list(APPEND compile_flags -fno-math-errno -march=x86-64-v3)
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
  if(Z_ENABLE_COMPILER_WARNING_EXTRA)
    list(APPEND warning_flags /Wall
                              )
  else()
    list(APPEND warning_flags /W4
                              )
  endif()
  if(Z_MAKE_WARNING_INTO_ERROR)
    list(APPEND warning_flags /WX)
  endif()

  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getMsvcWarningFlags)


function(Zisc_getClangClWarningFlags compile_warning_flags)
  set(warning_flags "")
  if(Z_ENABLE_COMPILER_WARNING_EXTRA)
    list(APPEND warning_flags /Wall
                              -Wno-c++-compat
                              -Wno-c++98-compat
                              -Wno-c++98-compat-pedantic
                              )
  else()
    list(APPEND warning_flags /W4
                              )
  endif()
  if(Z_MAKE_WARNING_INTO_ERROR)
    list(APPEND warning_flags /WX)
  endif()

  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getClangClWarningFlags)


function(Zisc_getClangWarningFlags compile_warning_flags)
  set(warning_flags "")
  if(Z_ENABLE_COMPILER_WARNING_EXTRA)
    list(APPEND warning_flags -Weverything
                              -Wno-c++-compat
                              -Wno-c++98-compat
                              -Wno-c++98-compat-pedantic
                              )
  else()
    list(APPEND warning_flags -Wall
                              -Wextra
                              -pedantic
                              )
  endif()
  if(Z_MAKE_WARNING_INTO_ERROR)
    list(APPEND warning_flags -Werror)
  endif()

  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getClangWarningFlags)


function(Zisc_getGccWarningFlags compile_warning_flags)
  set(warning_flags "")
  list(APPEND warning_flags -Wall
                            -Wextra
                            -pedantic
                            -Wno-attributes # GCC warns [[maybe_unused]] on member variable
                            )
  if(Z_ENABLE_COMPILER_WARNING_EXTRA)
    list(APPEND warning_flags -Wcast-align
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
  endif()
  if(Z_MAKE_WARNING_INTO_ERROR)
    list(APPEND warning_flags -Werror)
  endif()

  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getGccWarningFlags)


# Output functions
# Get compile options
function(Zisc_getCxxCompilerFlags cxx_compile_flags cxx_linker_flags cxx_definitions)
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

  if(Z_ENABLE_RECENT_HARDWARE_FEATURES)
    list(APPEND definitions Z_ENABLE_RECENT_HARDWARE_FEATURES=1)
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

  # Parse arguments
  set(options "")
  set(one_value_args "")
  set(multi_value_args CLANG_TIDY_EXCLUSION_CHECKS)
  cmake_parse_arguments(PARSE_ARGV 1 ZISC "${options}" "${one_value_args}" "${multi_value_args}")

  get_target_property(binary_dir ${target} BINARY_DIR)
  cmake_path(SET analyzation_dir "${binary_dir}/Analyzation/${target}")

  # Set visual studio analyzer
  Zisc_setCompilerStaticAnalyzer(${target} "${analyzation_dir}")
  list(APPEND static_analyzer_list "analyzer")

  # clang-tidy
  if(Z_ENABLE_STATIC_ANALYZER_CLANG_TIDY AND Z_CLANG)
    find_program(clang_tidy "clang-tidy")
    if(clang_tidy)
      set(tidy_command "${clang_tidy}")
      set(check_list "")
      list(APPEND check_list bugprone-*
                             clang-analyzer-*
                             concurrency-*
                             misc-*
                             modernize-*
                             performance-*
                             portability-*
                             readability-*)
      set(checks "")
      foreach(check IN LISTS check_list)
        set(checks "${checks},${check}")
      endforeach(check)
      set(exclusion_list "")
      list(APPEND exclusion_list bugprone-easily-swappable-parameters
                                 modernize-use-equals-default
                                 modernize-use-trailing-return-type
                                 readability-braces-around-statements
                                 )
      list(APPEND exclusion_list ${ZISC_CLANG_TIDY_EXCLUSION_CHECKS})
      set(exclusion_checks "")
      foreach(exclusion_check IN LISTS exclusion_list)
        set(exclusion_checks "${exclusion_checks},-${exclusion_check}")
      endforeach(exclusion_check)
      list(APPEND tidy_command "-checks=-*${checks}${exclusion_checks}")
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

  if(Z_ENABLE_STATIC_ANALYZER_OPTIMIZATION)
    Zisc_setOptimizationStaticAnalyzer(${target} "${analyzation_dir}")
    list(APPEND static_analyzer_list "optimization")
  endif()

  message(STATUS "[${target}] Static analyzer: ${static_analyzer_list}")
endfunction(Zisc_setStaticAnalyzer)


function(Zisc_createLinkToTarget target output_dir)
  get_target_property(binary_dir ${target} BINARY_DIR)
  set(link_target ${target}_link)

  set(script
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n"
      "Zisc_createLinkToFiles(\"${output_dir}\" \"\${target_path}\")\n")
  cmake_path(SET script_dir "${binary_dir}/scripts")
  file(MAKE_DIRECTORY "${script_dir}")
  cmake_path(SET script_file "${script_dir}/${link_target}.cmake")
  file(WRITE "${script_file}" ${script})

  add_custom_target(
      ${link_target} ALL
      ${CMAKE_COMMAND} -D target_path=$<TARGET_FILE:${target}> -P "${script_file}"
      DEPENDS ${target}
      WORKING_DIRECTORY "${binary_dir}"
      COMMENT "Create a link to the target '${target}' into '${output_dir}'"
      SOURCE "${script_file}")
endfunction(Zisc_createLinkToTarget)


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


function(Zisc_populateTargetOptions source_target dest_target)
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake")

  Zisc_checkTarget(${source_target})
  Zisc_checkTarget(${dest_target})

  # TODO. Why it's needed?
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  get_target_property(zivc_compile_flags ${source_target} INTERFACE_COMPILE_OPTIONS)
  if(zivc_compile_flags)
    target_compile_options(${dest_target} PRIVATE ${zivc_compile_flags})
  endif()
  get_target_property(zivc_libraries ${source_target} INTERFACE_LINK_LIBRARIES)
  if(zivc_libraries)
    target_link_libraries(${dest_target} PRIVATE ${zivc_libraries})
  endif()
  get_target_property(zivc_linker_flags ${source_target} INTERFACE_LINK_OPTIONS)
  if(zivc_linker_flags)
    target_link_options(${dest_target} PRIVATE ${zivc_linker_flags})
  endif()
  get_target_property(zivc_definitions ${source_target} INTERFACE_COMPILE_DEFINITIONS)
  if(zivc_definitions)
    target_compile_definitions(${dest_target} PRIVATE ${zivc_definitions})
  endif()
  get_target_property(zivc_features ${source_target} INTERFACE_COMPILE_FEATURES)
  if(zivc_features)
    target_compile_features(${dest_target} PRIVATE ${zivc_features})
  endif()
  #  get_target_property(zivc_include_dirs ${source_target} INTERFACE_INCLUDE_DIRECTORIES)
  #  if(zivc_include_dirs)
  #    target_include_directories(${dest_target} PRIVATE ${zivc_include_dirs})
  #  endif()
  #  get_target_property(zivc_include_dirs ${source_target} INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
  #  if(zivc_include_dirs)
  #    target_include_directories(${dest_target} SYSTEM PRIVATE ${zivc_include_dirs})
  #  endif()
endfunction(Zisc_populateTargetOptions)


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

# Internal functinos

function(Zisc_getClangOptionSuffix suffix)
  set(option_suffix "")
  if(Z_VISUAL_STUDIO)
    set(option_suffix "/clang:")
  endif()

  # Output variable
  set(${suffix} "${option_suffix}" PARENT_SCOPE)
endfunction(Zisc_getClangOptionSuffix)


function(Zisc_setCompilerStaticAnalyzer target analyzation_dir)
  cmake_path(APPEND analyzer_dir "${analyzation_dir}" "analyzer")
  file(MAKE_DIRECTORY "${analyzer_dir}")
  set(analyzer_flags "")
  if(Z_CLANG)
    # Zisc_getClangOptionSuffix(suffix)
    # list(APPEND analyzer_flags ${suffix}--analyze
    # ${suffix}--analyzer-output html
    #                           )
  elseif(Z_MSVC)
    cmake_path(NATIVE_PATH analyzer_dir NORMALIZE vsanalyzer_dir)
    list(APPEND analyzer_flags /analyze
                               # "/analyze:log \"${vsanalyzer_dir}\\\""
                               )
  endif()
  target_compile_options(${target} PRIVATE ${analyzer_flags})
endfunction(Zisc_setCompilerStaticAnalyzer)


function(Zisc_setOptimizationStaticAnalyzer target analyzation_dir)
  get_target_property(binary_dir ${target} BINARY_DIR)
  cmake_path(APPEND optimization_dir "${analyzation_dir}" "optimization")

  # Save assembly files
  cmake_path(APPEND assembly_dir "${optimization_dir}" "assembly")
  file(MAKE_DIRECTORY "${assembly_dir}")
  ## Generate a script
  set(script "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n")
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
  ## Save the script
  set(assembly_target ${target}_assembly)
  cmake_path(SET script_dir "${optimization_dir}/script")
  file(MAKE_DIRECTORY "${script_dir}")
  cmake_path(SET script_file "${script_dir}/${assembly_target}.cmake")
  file(WRITE "${script_file}" ${script})

  add_custom_target(
      ${assembly_target} ALL
      ${CMAKE_COMMAND} -P "${script_file}"
      DEPENDS ${target}
      WORKING_DIRECTORY "${binary_dir}"
      COMMENT "Create links to the assembly of '${target}' into '${assembly_dir}'."
      SOURCES "${script_file}")

  if(Z_VISUAL_STUDIO)
    if(Z_MSVC)
      cmake_path(NATIVE_PATH assembly_dir NORMALIZE asm_dir)
      target_compile_options(${target} PRIVATE /FAs
                                               "/Fa${asm_dir}\\"
                                               )
    elseif(Z_CLANG)
      cmake_path(NATIVE_PATH assembly_dir NORMALIZE asm_dir)
      target_compile_options(${target} PRIVATE /FA
                                               "/Fa${asm_dir}/"
                                               )
    endif()
  endif()

  # Save optimization report
  if(Z_CLANG)
    cmake_path(SET opt_file_path "${optimization_dir}/optimization_report.yaml")
    cmake_path(NATIVE_PATH opt_file_path NORMALIZE file_path)
    Zisc_getClangOptionSuffix(suffix)
    target_compile_options(${target} PRIVATE ${suffix}-fsave-optimization-record
                                             ${suffix}-foptimization-record-file=${file_path})
  endif()
  if(Z_MSVC)
    # target_compile_options(${target} PRIVATE /Qpar-report:2 /Qvec-report:2)
  endif()
endfunction(Zisc_setOptimizationStaticAnalyzer)


function(Zisc_createLinkToFiles output_dir)
  foreach(file_path IN LISTS ARGN)
    cmake_path(GET file_path FILENAME file_name)
    cmake_path(APPEND link_path "${output_dir}" "${file_name}")
    cmake_path(COMPARE "${link_path}" NOT_EQUAL "${file_path}" result)
    if(result)
      message(STATUS "Create a link '${file_name}'.")
      file(CREATE_LINK "${file_path}" "${link_path}" RESULT result COPY_ON_ERROR)
      if(result)
        message(FATAL_ERROR "${result}")
      endif()
    endif()
  endforeach(file_path)
endfunction(Zisc_createLinkToFiles)


function(Zisc_createLinkToTargetTempFiles target binary_dir output_dir)
  find_file(tmp_dir
      "${target}.dir"
      PATHS "${binary_dir}"
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
