# file: compiler.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2022 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

include("${CMAKE_CURRENT_LIST_DIR}/compiler_internal.cmake")


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


# Get compile options
function(Zisc_getCxxCompilerFlags architecture cxx_compile_flags cxx_linker_flags cxx_definitions)
  if(Z_GCC)
    Zisc_getGccCompilerFlags(${architecture} compile_flags linker_flags definitions)
  elseif(Z_CLANG AND Z_VISUAL_STUDIO)
    Zisc_getClangClCompilerFlags(${architecture} compile_flags linker_flags definitions)
  elseif(Z_CLANG)
    Zisc_getClangCompilerFlags(${architecture} compile_flags linker_flags definitions)
  elseif(Z_MSVC)
    Zisc_getMsvcCompilerFlags(${architecture} compile_flags linker_flags definitions)
  endif()

  if(Z_ENABLE_HARDWARE_FEATURES)
    list(APPEND definitions Z_ENABLE_HARDWARE_FEATURES=1)
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
    elseif(Z_CLANG AND Z_VISUAL_STUDIO)
      Zisc_getClangClWarningFlags(warning_flags)
    elseif(Z_CLANG)
      Zisc_getClangWarningFlags(warning_flags)
    elseif(Z_MSVC)
      Zisc_getMsvcWarningFlags(warning_flags)
    else()
      message(WARNING "${environment}: Warning option isn't supported.")
    endif()
  endif()

  # Output variables
  set(${compile_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getCxxWarningFlags)


function(Zisc_getArchitectureName only_representative arch_name_list)
  set(name_list "")
  if(Z_AMD64)
    Zisc_getArchitectureNameAmd64(${only_representative} name_list)
  endif()


  # Output variables
  set(${arch_name_list} ${name_list} PARENT_SCOPE)
endfunction(Zisc_getArchitectureName)


function(Zisc_getArchitectureTargetName project_name arch_name target_name)
  set(${target_name} "${project_name}-${arch_name}" PARENT_SCOPE)
endfunction(Zisc_getArchitectureTargetName)


#
function(Zisc_setStaticAnalyzer target)
  set(static_analyzer_list "")

  # Parse arguments
  set(options "")
  set(one_value_args "")
  set(multi_value_args CLANG_TIDY_EXCLUSION_CHECKS)
  cmake_parse_arguments(PARSE_ARGV 1 ZISC "${options}" "${one_value_args}" "${multi_value_args}")

  # Set output directory
  get_target_property(binary_dir ${target} BINARY_DIR)
  cmake_path(SET analyzation_dir "${binary_dir}/Analyzation/${target}")

  # Set compiler's static analyzer
  Zisc_setCompilerStaticAnalyzer(${target} "${analyzation_dir}")
  list(APPEND static_analyzer_list "compiler")

  # clang-tidy
  if(Z_ENABLE_STATIC_ANALYZER_CLANG_TIDY AND Z_CLANG)
    Zisc_setClangTidyAnalyzer(${target} "${ZISC_CLANG_TIDY_EXCLUSION_CHECKS}")
    list(APPEND static_analyzer_list "clang-tidy")
  endif()

  if(Z_ENABLE_STATIC_ANALYZER_LWYU)
    set_target_properties(${target} PROPERTIES LINK_WHAT_YOU_USE TRUE)
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
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/compiler_internal.cmake\")\n"
      "Zisc_createLinkToFiles(\"${output_dir}\" \"\${target_path}\")\n")
  cmake_path(SET script_dir "${binary_dir}/Script")
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


function(Zisc_populateTargetOptions source_target dest_target)
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake")

  Zisc_checkTarget(${source_target})
  Zisc_checkTarget(${dest_target})

  # TODO. Why it's needed?
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  get_target_property(zisc_cxx_standard ${source_target} CXX_STANDARD)
  if(zisc_cxx_standard)
    set_target_properties(${dest_target} PROPERTIES CXX_STANDARD ${zisc_cxx_standard}
                                                    CXX_STANDARD_REQUIRED ON)
  endif()
  get_target_property(zisc_compile_flags ${source_target} INTERFACE_COMPILE_OPTIONS)
  if(zisc_compile_flags)
    target_compile_options(${dest_target} PRIVATE ${zisc_compile_flags})
  endif()
  get_target_property(zisc_libraries ${source_target} INTERFACE_LINK_LIBRARIES)
  if(zisc_libraries)
    target_link_libraries(${dest_target} PRIVATE ${zisc_libraries})
  endif()
  get_target_property(zisc_linker_flags ${source_target} INTERFACE_LINK_OPTIONS)
  if(zisc_linker_flags)
    target_link_options(${dest_target} PRIVATE ${zisc_linker_flags})
  endif()
  get_target_property(zisc_definitions ${source_target} INTERFACE_COMPILE_DEFINITIONS)
  if(zisc_definitions)
    target_compile_definitions(${dest_target} PRIVATE ${zisc_definitions})
  endif()
  get_target_property(zisc_features ${source_target} INTERFACE_COMPILE_FEATURES)
  if(zisc_features)
    target_compile_features(${dest_target} PRIVATE ${zisc_features})
  endif()
  get_target_property(zisc_includes ${source_target} INTERFACE_INCLUDE_DIRECTORIES)
  if(zisc_includes)
    target_include_directories(${dest_target} PRIVATE ${zisc_includes})
  endif()
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
