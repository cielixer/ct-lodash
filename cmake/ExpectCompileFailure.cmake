# ExpectCompileFailure.cmake
# Utility to create negative compile tests that verify expected compilation failures
#
# Usage:
#   expect_compile_failure(
#     TARGET target_name
#     SOURCE source_file.cpp
#     EXPECTED_DIAGNOSTIC "substring that must appear in compiler error"
#   )

function(expect_compile_failure)
  set(options "")
  set(oneValueArgs TARGET SOURCE EXPECTED_DIAGNOSTIC)
  set(multiValueArgs "")
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "expect_compile_failure: TARGET is required")
  endif()
  if(NOT ARG_SOURCE)
    message(FATAL_ERROR "expect_compile_failure: SOURCE is required")
  endif()
  if(NOT ARG_EXPECTED_DIAGNOSTIC)
    message(FATAL_ERROR "expect_compile_failure: EXPECTED_DIAGNOSTIC is required")
  endif()

  # Create a unique object file path for this test
  set(obj_file "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}.obj")
  set(log_file "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}.log")

  # Add a test that attempts to compile the source and expects failure
  add_test(
    NAME ${ARG_TARGET}
    COMMAND ${CMAKE_COMMAND}
      -DSOURCE_FILE=${ARG_SOURCE}
      -DOBJ_FILE=${obj_file}
      -DLOG_FILE=${log_file}
      -DEXPECTED_DIAGNOSTIC=${ARG_EXPECTED_DIAGNOSTIC}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -DCXX_STANDARD=23
      -DINCLUDE_DIR=${CMAKE_SOURCE_DIR}/include
      -DADDITIONAL_INCLUDE_DIRS=${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
      -P ${CMAKE_CURRENT_FUNCTION_LIST_FILE}
  )

  set_tests_properties(${ARG_TARGET} PROPERTIES
    LABELS "negative"
  )
endfunction()

# When invoked as a script (via -P flag), perform the compilation attempt
if(CMAKE_SCRIPT_MODE_FILE)
  # Construct compiler invocation
  set(compile_command
    ${CMAKE_CXX_COMPILER}
    -std=c++${CXX_STANDARD}
    -DCTL_NEGATIVE_TEST=1
    -I${INCLUDE_DIR}
    -c ${SOURCE_FILE}
    -o ${OBJ_FILE}
  )

  foreach(extra_include_dir IN LISTS ADDITIONAL_INCLUDE_DIRS)
    list(APPEND compile_command -isystem ${extra_include_dir})
  endforeach()

  # Attempt compilation and capture output
  execute_process(
    COMMAND ${compile_command}
    RESULT_VARIABLE compile_result
    OUTPUT_VARIABLE compile_output
    ERROR_VARIABLE compile_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
  )

  # Combine stdout and stderr
  set(full_output "${compile_output}\n${compile_error}")

  # Write log file
  file(WRITE ${LOG_FILE} "Compile command: ${compile_command}\n")
  file(APPEND ${LOG_FILE} "Exit code: ${compile_result}\n")
  file(APPEND ${LOG_FILE} "Output:\n${full_output}\n")

  # Verify compilation failed
  if(compile_result EQUAL 0)
    message(FATAL_ERROR "Expected compilation failure, but compilation succeeded")
  endif()

  # Verify expected diagnostic appears
  string(FIND "${full_output}" "${EXPECTED_DIAGNOSTIC}" diagnostic_pos)
  if(diagnostic_pos EQUAL -1)
    message(FATAL_ERROR 
      "Expected diagnostic not found in compiler output.\n"
      "Expected substring: ${EXPECTED_DIAGNOSTIC}\n"
      "Full output:\n${full_output}"
    )
  endif()

  # Success: compilation failed with expected diagnostic
  message(STATUS "Negative test passed: compilation failed as expected with '${EXPECTED_DIAGNOSTIC}'")
endif()
