# file: compiler_internal.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2023 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


#
function(Zisc_getClangOptionSuffix suffix)
  set(option_suffix "")
  if(Z_VISUAL_STUDIO)
    set(option_suffix "/clang:")
  endif()

  # Output variable
  set(${suffix} "${option_suffix}" PARENT_SCOPE)
endfunction(Zisc_getClangOptionSuffix)


#
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
    if(NOT Z_DEBUG_MODE)
      list(APPEND check_list object-size
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
      list(APPEND compile_flags "-fsanitize=${check_flag}"
                                -fno-omit-frame-pointer
                                )
      list(APPEND linker_flags "-fsanitize=${check_flag}")
    endif()

    # Output
    set(${compile_sanitizer_flags} ${compile_flags} PARENT_SCOPE)
    set(${linker_sanitizer_flags} ${linker_flags} PARENT_SCOPE)
  endif()
endfunction(Zisc_getSanitizerFlags)


function(Zisc_getMsvcCompilerFlags architecture cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  # Architecture optimization
  if(architecture MATCHES "Amd64")
    list(APPEND compile_flags /favor:AMD64)
    if(architecture MATCHES "Amd64-v3")
      list(APPEND compile_flags /arch:AVX2)
    elseif(architecture MATCHES "Amd64-v4")
      list(APPEND compile_flags /arch:AVX512)
    endif()
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


function(Zisc_getClangClCompilerFlags architecture cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  # Architecture optimization
  if(architecture MATCHES "Amd64-v1")
    list(APPEND compile_flags /clang:-march=x86-64)
  elseif(architecture MATCHES "Amd64-v2")
    list(APPEND compile_flags /clang:-fno-math-errno
                              /clang:-march=x86-64-v2)
  elseif(architecture MATCHES "Amd64-v3")
    list(APPEND compile_flags /clang:-fno-math-errno
                              /clang:-march=x86-64-v3)
  elseif(architecture MATCHES "Amd64-v4")
    list(APPEND compile_flags /clang:-fno-math-errno
                              /clang:-march=x86-64-v4)
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


function(Zisc_getClangCompilerFlags architecture cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  # Architecture optimization
  if(architecture MATCHES "Amd64-v1")
    list(APPEND compile_flags -march=x86-64)
  elseif(architecture MATCHES "Amd64-v2")
    list(APPEND compile_flags -fno-math-errno
                              -march=x86-64-v2)
  elseif(architecture MATCHES "Amd64-v3")
    list(APPEND compile_flags -fno-math-errno
                              -march=x86-64-v3)
  elseif(architecture MATCHES "Amd64-v4")
    list(APPEND compile_flags -fno-math-errno
                              -march=x86-64-v4)
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


function(Zisc_getGccCompilerFlags architecture cxx_compile_flags cxx_linker_flags cxx_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  # Architecture optimization
  if(architecture MATCHES "Amd64-v1")
    list(APPEND compile_flags -march=x86-64)
  elseif(architecture MATCHES "Amd64-v2")
    list(APPEND compile_flags -fno-math-errno
                              -march=x86-64-v2)
  elseif(architecture MATCHES "Amd64-v3")
    list(APPEND compile_flags -fno-math-errno
                              -march=x86-64-v3)
  elseif(architecture MATCHES "Amd64-v4")
    list(APPEND compile_flags -fno-math-errno
                              -march=x86-64-v4)
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


function(Zisc_getArchitectureNameAmd64 only_representative arch_name_list)
  set(name_list "Amd64-v1")
  if(Z_ENABLE_HARDWARE_FEATURES)
    list(APPEND name_list "Amd64-v2" "Amd64-v3" "Amd64-v4")
    if(only_representative)
      list(GET name_list 2 representative) # v3
      set(name_list ${representative})
    endif()
  endif()

  # Output variables
  set(${arch_name_list} ${name_list} PARENT_SCOPE)
endfunction(Zisc_getArchitectureNameAmd64)


#
function(Zisc_setClangTidyAnalyzer target exclusion_checks)
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
    list(APPEND exclusion_list ${exclusion_checks})
    set(exclusion_checks "")
    foreach(exclusion_check IN LISTS exclusion_list)
      set(exclusion_checks "${exclusion_checks},-${exclusion_check}")
    endforeach(exclusion_check)
    list(APPEND tidy_command "-checks=-*${checks}${exclusion_checks}")
    set_target_properties(${target} PROPERTIES
        C_CLANG_TIDY "${tidy_command}"
        CXX_CLANG_TIDY "${tidy_command}")
  else()
    message(WARNING "[${target}] Could not find 'clang-tidy'.")
  endif()
endfunction(Zisc_setClangTidyAnalyzer)


function(Zisc_setCompilerStaticAnalyzer target analyzation_dir)
  cmake_path(APPEND compiler_dir "${analyzation_dir}" "compiler")
  file(MAKE_DIRECTORY "${compiler_dir}")
  set(compiler_flags "")
  if(Z_CLANG) # TODO. The analyzer doesn't work
    # Zisc_getClangOptionSuffix(suffix)
    # list(APPEND compiler_flags ${suffix}--analyze
    #                            ${suffix}--analyzer-output html
    #                            )
  elseif(Z_MSVC)
    cmake_path(NATIVE_PATH compiler_dir NORMALIZE vscompiler_dir)
    list(APPEND compiler_flags /analyze
                               # "/analyze:log \"${vscompiler_dir}\\\""
                               )
  endif()
  target_compile_options(${target} PRIVATE ${compiler_flags})
endfunction(Zisc_setCompilerStaticAnalyzer)


function(Zisc_setOptimizationStaticAnalyzer target analyzation_dir)
  get_target_property(binary_dir ${target} BINARY_DIR)
  cmake_path(APPEND optimization_dir "${analyzation_dir}" "optimization")

  set(compile_flags "")

  # Save assembly files
  cmake_path(APPEND assembly_dir "${optimization_dir}" "assembly")
  file(MAKE_DIRECTORY "${assembly_dir}")

  if(Z_LINUX OR Z_MAC)
    # Assume clang or gcc
    list(APPEND compile_flags -save-temps=obj
                              -fverbose-asm
                              )

    ## Generate a script
    set(script "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n")
    # Search assembly files
    string(APPEND script
        "Zisc_createLinkToTargetTempFiles(\"${target}\" \"${binary_dir}\" \"${assembly_dir}\" s)\n")
    if(Z_CLANG)
      # Search bitcode files
      string(APPEND script
          "Zisc_createLinkToTargetTempFiles(\"${target}\" \"${binary_dir}\" \"${assembly_dir}\" bc)\n")
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
  endif()

  if(Z_VISUAL_STUDIO)
    set(fa_options "")
    if(Z_MSVC)
      set(fa_options "s")
    endif()
    cmake_path(NATIVE_PATH assembly_dir NORMALIZE asm_dir)
    list(APPEND compile_flags "/FA${fa_options}"
                              "/Fa${asm_dir}\\"
                              )
  endif()

  # Save optimization report
  if(Z_CLANG)
    cmake_path(SET opt_file_path "${optimization_dir}/optimization_report.yaml")
    cmake_path(NATIVE_PATH opt_file_path NORMALIZE file_path)
    Zisc_getClangOptionSuffix(suffix)
    list(APPEND compile_flags ${suffix}-fsave-optimization-record
                              ${suffix}-foptimization-record-file=${file_path}
                              )
  endif()
  if(Z_MSVC)
    # TODO. This report overflows console log
    # list(APPEND compile_flags /Qpar-report:2 /Qvec-report:2)
  endif()

  #
  target_compile_options(${target} PRIVATE ${compile_flags})
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
