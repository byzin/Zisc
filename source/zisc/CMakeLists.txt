# file:CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.30)


function(Zisc_initZiscOptions)
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/general.cmake")

  set(option_description "Use OS specified atomic wait-notification.")
  Zisc_setBooleanOption(ZISC_ATOMIC_OS_SPECIFIED_WAIT ON "${option_description}")

  set(option_description "Enable zisc assertion.")
  if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    Zisc_setBooleanOption(ZISC_ENABLE_ASSERTION ON "${option_description}")
  else()
    Zisc_setBooleanOption(ZISC_ENABLE_ASSERTION OFF "${option_description}")
  endif()

  set(option_description "L1 cache line size")
  Zisc_setStringOption(ZISC_L1_CACHE_LINE_SIZE "128" "${option_description}")
endfunction(Zisc_initZiscOptions)


# Set Zisc specific compile flags to the given target
function(Zisc_setZiscFlags target scope)
  # Include dependencies
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/general.cmake")

  Zisc_checkTarget(${target})

  # Actually set the definitions
  target_compile_definitions(${target} ${scope}
    ZISC_ATOMIC_OS_SPECIFIED_WAIT=$<BOOL:${ZISC_ATOMIC_OS_SPECIFIED_WAIT}>
    ZISC_ASSERTION=$<BOOL:${ZISC_ENABLE_ASSERTION}>
    $<$<PLATFORM_ID:Windows>:_ENABLE_EXTENDED_ALIGNED_STORAGE=1>
  )
endfunction(Zisc_setZiscFlags)


#
function(Zisc_setZiscWarningFlags target scope)
  # Include dependencies
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/general.cmake")

  Zisc_checkTarget(${target})

  # Set properties
  set(has_msvc $<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>)
  set(has_gcc $<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>)
  set(has_clang $<OR:$<C_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)

  # Set MSVC warnings
  set(msvc_flags # /wd4244 # 'argument': conversion from '_Ty' to 'const int', possible loss of data¬
                 # /wd4267 # conversion from 'size_t' to '_To', possible loss of data.
                 # /wd4324 # structure was padded due to alignment specifier
               )

  # Set GCC warnings
  set(gcc_flags)

  # Set Clang warnings
  set(clang_flags)

  # Actually set the warnings
  target_compile_options(${target} ${scope}
    $<${has_msvc}:${msvc_flags}>
    $<${has_gcc}:${gcc_flags}>
    $<${has_clang}:${clang_flags}>
  )
endfunction(Zisc_setZiscWarningFlags)


# Set Zisc library project
macro(Zisc_setZiscProject)
  # Set the project info
  set(project_description "Zisc provides commonly used C++ classes and functions.")
  project(Zisc VERSION ${ZiscProject_VERSION} DESCRIPTION "${project_description}" LANGUAGES CXX)
  message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")


  # Include dependencies
  include("${PROJECT_SOURCE_DIR}/cmake/general.cmake")
  include("${PROJECT_SOURCE_DIR}/cmake/platform.cmake")
  include("${PROJECT_SOURCE_DIR}/cmake/compiler.cmake")


  # Initialize zisc options
  Zisc_initCompilerOptions()
  Zisc_initZiscOptions()


  # Find dependency modules
  if(NOT TARGET Threads::Threads)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
  endif()


  Zisc_getFeatureLevelList(feature_level_list)
  # Create build property targets 
  set(build_properties_target_base "ZiscBuildProperties")
  message(STATUS "feature level list: ${feature_level_list}")
  foreach(feature_level IN LISTS feature_level_list)
    Zisc_suffixFeatureLevelName("zisc_build_properties" ${feature_level} target)
    add_library(${target} INTERFACE)
    Zisc_setPlatformFlags(${target} INTERFACE)
    Zisc_setCxxCompileFlags(${target} ${feature_level} INTERFACE)
    Zisc_setZiscFlags(${target} INTERFACE)
    target_link_libraries(${target} INTERFACE Threads::Threads)
    # Create an alias
    add_library(${PROJECT_NAME}::${build_properties_target_base}${feature_level} ALIAS ${target})
  endforeach(feature_level)


  # Create zisc configuration header file
  cmake_path(SET zisc_core_dir "${PROJECT_SOURCE_DIR}/core")
  cmake_path(SET zisc_config_dir "${PROJECT_BINARY_DIR}/include/zisc")
  file(MAKE_DIRECTORY "${zisc_config_dir}")
  cmake_path(SET zisc_config_file "${zisc_config_dir}/zisc_config-inl.hpp")
  configure_file("${zisc_core_dir}/zisc/zisc_config-inl.hpp.in"
                 "${zisc_config_file}"
                 @ONLY)

  # Gather zisc source files
  file(GLOB_RECURSE zisc_source_files ${zisc_core_dir}/zisc/*[.hpp|.cpp])
  source_group(TREE "${zisc_core_dir}/zisc" PREFIX ${PROJECT_NAME} FILES "${zisc_source_files}")
  list(APPEND zisc_source_files "${zisc_config_file}")
  source_group(TREE "${zisc_config_dir}" PREFIX ${PROJECT_NAME} FILES "${zisc_config_file}")

  # Create zisc libraries
  set(zisc_target_base "Zisc")
  foreach(feature_level IN LISTS feature_level_list)
    # Check build property target
    set(build_property_target ${PROJECT_NAME}::${build_properties_target_base}${feature_level})
    Zisc_checkTarget(${build_property_target})
    # Zisc
    Zisc_suffixFeatureLevelName("zisc" ${feature_level} target)
    add_library(${target} STATIC "${zisc_source_files}")
    target_link_libraries(${target} PUBLIC ${build_property_target})
    Zisc_populateTargetCompilationProperties(${build_property_target} ${target})
    set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    Zisc_setSanitizerFlags(${target} PRIVATE)
    Zisc_setCxxWarningFlags(${target} PRIVATE)
    Zisc_setZiscWarningFlags(${target} PRIVATE)
    Zisc_enableIpo(${target})
    target_include_directories(${target} PUBLIC "${zisc_core_dir}"
                                                "${PROJECT_BINARY_DIR}/include")

    # TODO. Remove me
    target_link_libraries(${target} PRIVATE $<$<PLATFORM_ID:Windows>:Synchronization.lib>)
    target_compile_definitions(${target} PRIVATE $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>)

    # Set static analyzer
    set(clang_tidy_exclusion_checks 
                                    bugprone-easily-swappable-parameters
                                    cppcoreguidelines-avoid-do-while
                                    cppcoreguidelines-avoid-magic-numbers
                                    cppcoreguidelines-macro-usage
                                    cppcoreguidelines-no-malloc
                                    cppcoreguidelines-owning-memory
                                    cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay
                                    cppcoreguidelines-pro-bounds-constant-array-index
                                    cppcoreguidelines-pro-bounds-pointer-arithmetic
                                    cppcoreguidelines-pro-type-const-cast
                                    cppcoreguidelines-pro-type-reinterpret-cast
                                    cppcoreguidelines-pro-type-vararg
                                    cppcoreguidelines-special-member-functions
                                    google-explicit-constructor
                                    google-readability-braces-around-statements
                                    google-runtime-int
                                    hicpp-braces-around-statements
                                    hicpp-explicit-conversions
                                    hicpp-no-array-decay
                                    hicpp-no-malloc
                                    hicpp-signed-bitwise
                                    hicpp-special-member-functions
                                    hicpp-vararg
                                    misc-include-cleaner
                                    performance-avoid-endl
                                    readability-avoid-const-params-in-decls
                                    readability-braces-around-statements
                                    readability-else-after-return
                                    readability-function-cognitive-complexity
                                    readability-identifier-length
                                    readability-magic-numbers
                                    )
    Zisc_setStaticAnalyzer(${target}
                           CLANG_TIDY_HEADER_PATHS ${PROJECT_SOURCE_DIR}/core/zisc/
                           CLANG_TIDY_EXCLUSION_CHECKS ${clang_tidy_exclusion_checks})

    Zisc_printTargetCompileProperties(${target} "  + ")

    # Create an alias
    add_library(${PROJECT_NAME}::${zisc_target_base}${feature_level} ALIAS ${target})
  endforeach(feature_level)
endmacro(Zisc_setZiscProject)


##
Zisc_setZiscProject()
