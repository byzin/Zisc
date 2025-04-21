# file: compiler.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


include_guard()


#
function(Zisc_initCompilerOptions)
  # Include dependencies
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake")

  set(description "Enable compiler recommended warnings.")
  Zisc_setBooleanOption(Z_ENABLE_COMPILER_WARNING ON "${description}")

  set(description "Enable compiler full warnings (if compiler supports that).")
  Zisc_setBooleanOption(Z_ENABLE_COMPILER_WARNING_EXTRA OFF "${description}")

  set(description "Make compiler warnings into errors (if supported that).")
  Zisc_setBooleanOption(Z_MAKE_WARNING_INTO_ERROR OFF "${description}")

  set(description "Clang uses LLVM's build tools and libraries instead of platform specific tools.")
  Zisc_setBooleanOption(Z_CLANG_USES_LLVM_TOOLS OFF "${description}")

  set(description "Enable multiple hardware featured build.")
  Zisc_setBooleanOption(Z_ENABLE_HARDWARE_FEATURES OFF "${description}")

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


# Set compile options for C++ to the given target
function(Zisc_setCxxCompileFlags target feature_level scope)
  # Include dependencies
  include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.cmake)

  Zisc_checkIFFeatureLevelIsSupported(${feature_level})

  # Used options
  set(has_hardware_feature $<BOOL:${Z_ENABLE_HARDWARE_FEATURES}>)
  set(has_clang_tools $<BOOL:${Z_CLANG_USES_LLVM_TOOLS}>)

  # Set properties
  set(has_msvc $<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>)
  set(has_gcc $<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>)
  set(has_clang $<OR:$<C_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)
  set(has_apple_clang $<OR:$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)
  set(has_msvc_frontend 0)
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(has_msvc_frontend 1)
  endif()

  # Shared options
  set(definitions)

  # Set MSVC compile options
  set(msvc_flags # Diagnostic
                 /diagnostics:caret
                 /nologo
                 /fastfail
                 /options:strict
                 )
  set(msvc_linker_flags)
  set(msvc_definitions)

  # Set GCC compile options
  set(gcc_flags)
  set(gcc_linker_flags)
  set(gcc_definitions)

  # Set Clang compiler options
  set(clang_flags $<${has_clang_tools}:-stdlib=libc++>)
  set(clang_linker_flags $<${has_clang_tools}:-stdlib=libc++;-rtlib=compiler-rt>
                         $<$<AND:${has_clang_tools},$<NOT:${has_apple_clang}>>:-fuse-ld=lld>
                         )
  set(clang_definitions $<${has_clang_tools}:Z_CLANG_USES_LLVM_TOOLS>)

  # Set ClangCL compile options
  set(clang_cl_flags /Qvec # Auto loop-vectorization
                     # Diagnostic
                     /diagnostics:caret
                     -fcolor-diagnostics
                     )
  set(clang_cl_linker_flags)
  set(clang_cl_definitions)

  # Set feature flags
  if(feature_level STREQUAL "Amd64V1")
    list(APPEND msvc_flags /favor:AMD64)
    list(APPEND gcc_flags -march=x86-64)
    list(APPEND clang_flags -march=x86-64)
    list(APPEND clang_cl_flags /clang:-march=x86-64)
  elseif(feature_level STREQUAL "Amd64V2")
    list(APPEND msvc_flags /favor:AMD64)
    list(APPEND gcc_flags -fno-math-errno
                          -march=x86-64-v2)
    list(APPEND clang_flags -fno-math-errno
                            -march=x86-64-v2)
    list(APPEND clang_cl_flags /clang:-fno-math-errno
                               /clang:-march=x86-64-v2)
  elseif(feature_level STREQUAL "Amd64V3")
    list(APPEND msvc_flags /favor:AMD64
                           /arch:AVX2)
    list(APPEND gcc_flags -fno-math-errno
                          -march=x86-64-v3)
    list(APPEND clang_flags -fno-math-errno
                            -march=x86-64-v3)
    list(APPEND clang_cl_flags /clang:-fno-math-errno
                               /clang:-march=x86-64-v3)
  elseif(feature_level STREQUAL "Amd64V4")
    list(APPEND msvc_flags /favor:AMD64
                           /arch:AVX512)
    list(APPEND gcc_flags -fno-math-errno
                          -march=x86-64-v4)
    list(APPEND clang_flags -fno-math-errno
                            -march=x86-64-v4)
    list(APPEND clang_cl_flags /clang:-fno-math-errno
                               /clang:-march=x86-64-v4)
  endif()
  list(APPEND definitions Z_ARCH_FEATURE_LEVEL_NAME=\"${feature_level}\")

  # Actually set the options to the target
  target_compile_features(${target} ${scope} cxx_std_23)
  target_compile_options(${target} ${scope}
    $<${has_msvc}:${msvc_flags}>
    $<${has_gcc}:${gcc_flags}>
    $<${has_clang}:$<IF:${has_msvc_frontend},${clang_cl_flags},${clang_flags}>>
  )
  target_link_options(${target} ${scope}
    $<${has_msvc}:${msvc_linker_flags}>
    $<${has_gcc}:${gcc_linker_flags}>
    $<${has_clang}:$<IF:${has_msvc_frontend},${clang_cl_linker_flags},${clang_linker_flags}>>
  )
  target_compile_definitions(${target} ${scope}
    ${definitions}
    $<${has_msvc}:${msvc_definitions}>
    $<${has_gcc}:${gcc_definitions}>
    $<${has_clang}:$<IF:${has_msvc_frontend},${clang_cl_definitions},${clang_definitions}>>
    Z_ENABLE_HARDWARE_FEATURES=${has_hardware_feature}
  )
endfunction(Zisc_setCxxCompileFlags)


# Set compiler warning options for C++ to the given target
function(Zisc_setCxxWarningFlags target scope)
  # Used options
  set(has_warning $<BOOL:${Z_ENABLE_COMPILER_WARNING}>)
  set(has_extra $<BOOL:${Z_ENABLE_COMPILER_WARNING_EXTRA}>)
  set(has_error $<BOOL:${Z_MAKE_WARNING_INTO_ERROR}>)

  #
  if(NOT has_warning)
    return()
  endif()

  # Set properties
  set(has_msvc $<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>)
  set(has_gcc $<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>)
  set(has_clang $<OR:$<C_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)

  # Set MSVC warning options
  set(msvc_options /W4)
  set(msvc_options_extra /Wall)
  set(msvc_options_error /WX)

  # Set gcc warning options
  set(gcc_options -Wall
                  -Wextra
                  -pedantic
                  -Wno-attributes # GCC warns [[maybe_unused]] on member variable
                  )
  set(gcc_options_extra ${gcc_options}
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
  set(gcc_options_error -Werror)

  # Set clang warning options
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(clang_options /W4)
    set(clang_options_extra /Wall -Wno-c++-compat -Wno-c++98-compat -Wno-c++98-compat-pedantic)
    set(clang_options_error /WX)
  elseif(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    set(clang_options -Wall -Wextra -pedantic)
    set(clang_options_extra -Weverything -Wno-c++-compat -Wno-c++98-compat -Wno-c++98-compat-pedantic)
    set(clang_options_error -Werror)
  endif()

  # Actually set the options to the target
  target_compile_options(${target} ${scope}
    $<${has_msvc}:$<IF:${has_extra},${msvc_options_extra},${msvc_options}>;$<${has_error}:${msvc_options_error}>>
    $<${has_gcc}:$<IF:${has_extra},${gcc_options_extra},${gcc_options}>;$<${has_error}:${gcc_options_error}>>
    $<${has_clang}:$<IF:${has_extra},${clang_options_extra},${clang_options}>;$<${has_error}:${clang_options_error}>>
  )
endfunction(Zisc_setCxxWarningFlags)


# Return the name suffixed with the given feature level
function(Zisc_suffixFeatureLevelName name feature_level output)
  # Include dependencies
  include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.cmake)

  Zisc_checkIFFeatureLevelIsSupported(${feature_level})

  string(TOLOWER "${feature_level}" level)
  set(${output} "${name}-${level}" PARENT_SCOPE) 
endfunction(Zisc_suffixFeatureLevelName)


#
function(Zisc_setStaticAnalyzer target)
  # Include dependencies
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake")
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/compiler_internal.cmake")

  Zisc_checkTarget(${target})

  # Used options
  set(enable_clang_tidy ${Z_ENABLE_STATIC_ANALYZER_CLANG_TIDY})
  set(enable_lwyu ${Z_ENABLE_STATIC_ANALYZER_LWYU})
  set(enable_optimization ${Z_ENABLE_STATIC_ANALYZER_OPTIMIZATION})

  set(static_analyzer_list "")

  # Parse arguments
  set(options)
  set(one_value_args)
  set(multi_value_args CLANG_TIDY_HEADER_PATHS CLANG_TIDY_EXCLUSION_CHECKS)
  cmake_parse_arguments(PARSE_ARGV 1 ZISC "${options}" "${one_value_args}" "${multi_value_args}")

  # Set output directory
  get_target_property(binary_dir ${target} BINARY_DIR)
  cmake_path(SET analyzation_dir "${binary_dir}/Analyzation/${target}")

  # clang-tidy
  if(enable_clang_tidy)
    Zisc_setClangTidyAnalyzer(${target} "${ZISC_CLANG_TIDY_HEADER_PATHS}" "${ZISC_CLANG_TIDY_EXCLUSION_CHECKS}")
    list(APPEND static_analyzer_list "clang-tidy")
  endif()

  # LWYU
  if(enable_lwyu)
    set_target_properties(${target} PROPERTIES LINK_WHAT_YOU_USE TRUE)
    list(APPEND static_analyzer_list "link-what-you-use")
  endif()

  #
  if(enable_optimization)
    Zisc_setOptimizationStaticAnalyzer(${target} "${analyzation_dir}")
    Zisc_setDisassemblyAnalyzer(${target} "${analyzation_dir}")
    list(APPEND static_analyzer_list "optimization")
  endif()

  message(STATUS "[${target}] Set static analyzers: ${static_analyzer_list}")
endfunction(Zisc_setStaticAnalyzer)


#
function(Zisc_setSanitizerFlags target scope)
  # Used options
  set(has_address $<BOOL:${Z_ENABLE_SANITIZER_ADDRESS}>)
  set(has_thread $<BOOL:${Z_ENABLE_SANITIZER_THREAD}>)
  set(has_memory $<BOOL:${Z_ENABLE_SANITIZER_MEMORY}>)
  set(has_undef $<BOOL:${Z_ENABLE_SANITIZER_UNDEF_BEHAVIOR}>)
  set(has_leak $<BOOL:${Z_ENABLE_SANITIZER_LEAK}>)
  set(has_cfi $<BOOL:${Z_ENABLE_SANITIZER_CFI}>)
  set(has_safe_stack $<BOOL:${Z_ENABLE_SANITIZER_SAFE_STACK}>)

  # Actually set the sanitizer options
  target_compile_options(${target} ${scope}
    $<${has_address}:-fsanitize=address;-fno-omit-frame-pointer>
    $<${has_thread}:-fsanitize=thread>
    $<${has_memory}:-fsanitize=memory;-fno-omit-frame-pointer>
    $<${has_undef}:-fsanitize=undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds,nullability>
    $<${has_leak}:-fsanitize=leak>
    $<${has_cfi}:-fsanitize=cfi>
    $<${has_safe_stack}:-fsanitize=safe-stack>
  )
  target_link_options(${target} ${scope}
    $<${has_address}:-fsanitize=address>
    $<${has_thread}:-fsanitize=thread>
    $<${has_memory}:-fsanitize=memory>
    $<${has_undef}:-fsanitize=undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds,nullability>
    $<${has_leak}:-fsanitize=leak>
    $<${has_cfi}:-fsanitize=cfi>
    $<${has_safe_stack}:-fsanitize=safe-stack>
  )
endfunction(Zisc_setSanitizerFlags)


#
function(Zisc_enableIpo target)
  # Include dependencies
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake")
  include(CheckIPOSupported)

  Zisc_checkTarget(${target})

  set(has_clang_tools ${Z_CLANG_USES_LLVM_TOOLS})

  # TODO. Fix me. 'check_ipo_supported()' won't work with clang
  if((CMAKE_CXX_COMPILER_ID STREQUAL Clang) AND has_clang_tools)
    message(STATUS "[${target}] Enable IPO/LTO.")
    target_compile_options(${target} PRIVATE -flto=$<IF:$<CONFIG:Release>,auto,thin>
                                             -fno-fat-lto-objects)
    return()
  endif()

  check_ipo_supported(RESULT result OUTPUT output LANGUAGES CXX)
  if(result)
    message(STATUS "[${target}] Enable IPO/LTO.")
    set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING "[${target}] IPO/LTO isn't supported.")
  endif()
endfunction(Zisc_enableIpo)
