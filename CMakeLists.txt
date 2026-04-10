cmake_minimum_required(VERSION 3.21)

project(runtime_code_obf VERSION 0.1.0 LANGUAGES CXX)

option(RUNTIME_OBF_BUILD_EXAMPLES "Build example targets" ON)
option(RUNTIME_OBF_BUILD_TESTS "Build unit and binary-inspection tests" ON)

add_library(runtime_obf INTERFACE)
add_library(runtime_obf::runtime_obf ALIAS runtime_obf)

target_include_directories(
  runtime_obf
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_compile_features(runtime_obf INTERFACE cxx_std_20)

if(MSVC)
  target_compile_options(runtime_obf INTERFACE /W4 /permissive-)
else()
  target_compile_options(runtime_obf INTERFACE -Wall -Wextra -Wpedantic)
endif()

if(RUNTIME_OBF_BUILD_EXAMPLES)
  add_executable(runtime_obf_basic_example examples/basic.cpp)
  target_link_libraries(runtime_obf_basic_example PRIVATE runtime_obf::runtime_obf)
endif()

if(RUNTIME_OBF_BUILD_TESTS)
  enable_testing()

  find_package(Python3 REQUIRED COMPONENTS Interpreter)

  add_executable(runtime_obf_tests tests/obfuscated_string_tests.cpp)
  target_link_libraries(runtime_obf_tests PRIVATE runtime_obf::runtime_obf)

  add_test(NAME runtime_obf.unit COMMAND runtime_obf_tests)

  if(TARGET runtime_obf_basic_example)
    add_test(
      NAME runtime_obf.plaintext_scan
      COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_plaintext.py
        $<TARGET_FILE:runtime_obf_basic_example>
        "hunter2"
        "compile-time secret"
        "api-key-123")
  endif()
endif()

install(
  DIRECTORY include/
  DESTINATION include)
