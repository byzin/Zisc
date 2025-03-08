# file: compiler_internal.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


include_guard()


#
function(Zisc_setClangTidyAnalyzer target header_paths exclusion_checks)
  set(description "clang-based C++ 'linter' tool.")
  find_program(clang_tidy "clang-tidy" DOC ${description})
  if(NOT clang_tidy)
    message(WARNING "[${target}] Could not find 'clang-tidy'.")
    return()
  endif()

  set(tidy_command "${clang_tidy}")

  # Add header paths
  if(header_paths)
    list(APPEND tidy_command "--header-filter=${header_paths}")
  endif()

  # Add a check list
  set(check_list "")
  list(APPEND check_list bugprone-*
                         clang-analyzer-*
                         concurrency-*
                         cppcoreguidelines-*
                         google-*
                         hicpp-*
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
  list(APPEND exclusion_list ${exclusion_checks})
  set(exclusion_checks "")
  foreach(exclusion_check IN LISTS exclusion_list)
    set(exclusion_checks "${exclusion_checks},-${exclusion_check}")
  endforeach(exclusion_check)
  list(APPEND tidy_command "--checks=-*${checks}${exclusion_checks}")

  # Actually set the clang-tidy options
  set_target_properties(${target} PROPERTIES
      C_CLANG_TIDY "${tidy_command}"
      CXX_CLANG_TIDY "${tidy_command}")
endfunction(Zisc_setClangTidyAnalyzer)


#
function(Zisc_setOptimizationStaticAnalyzer target analyzation_dir)
  # Create the output directory
  cmake_path(APPEND optimization_dir "${analyzation_dir}" "optimization")
  file(MAKE_DIRECTORY "${optimization_dir}")

  # List of compile flags will be created
  set(compile_flags "")

  # Clang optimization record
  set(clang_list "Clang" "AppleClang")
  if(CMAKE_CXX_COMPILER_ID IN_LIST clang_list)
    # Set the output location of the records
    cmake_path(APPEND report_dir "${optimization_dir}" "report")
    file(MAKE_DIRECTORY "${report_dir}")
    #
    list(APPEND compile_flags -fsave-optimization-record)
    # Copy the records to the output directory
    set(object_file_list $<TARGET_OBJECTS:${target}>)
    set(object_file_location $<PATH:GET_PARENT_PATH,$<LIST:GET,${object_file_list},0>>)
    set(record_file_list $<LIST:TRANSFORM,${object_file_list},REPLACE,\.o$,.opt.yaml>)
    file(RELATIVE_PATH report_relative_dir ${CMAKE_SOURCE_DIR} ${report_dir})
    add_custom_command(TARGET ${target}
                       POST_BUILD
                       COMMENT "Collect the optimization records into '${report_relative_dir}'."
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${record_file_list} ${report_dir}
                       VERBATIM
                       COMMAND_EXPAND_LISTS
                      )
    # TODO. Adding custom command using 'llvm-opt-report'?
  endif()

  # Actually set the flags
  target_compile_options(${target} PRIVATE ${compile_flags})
endfunction(Zisc_setOptimizationStaticAnalyzer)


#
function(Zisc_setDisassemblyAnalyzer target analyzation_dir)
  # Create the output directory
  get_target_property(binary_dir ${target} BINARY_DIR)
  cmake_path(APPEND disassembly_dir "${analyzation_dir}" "disassembly")
  file(MAKE_DIRECTORY "${disassembly_dir}")

  # TODO. Implement me
endfunction(Zisc_setDisassemblyAnalyzer)
