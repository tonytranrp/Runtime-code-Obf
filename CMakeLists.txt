cmake_minimum_required(VERSION 3.21)

project(runtime_code_obf VERSION 0.1.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

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

function(runtime_obf_enable_warnings target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

if(RUNTIME_OBF_BUILD_EXAMPLES)
  add_executable(runtime_obf_basic_example examples/basic.cpp)
  target_link_libraries(runtime_obf_basic_example PRIVATE runtime_obf::runtime_obf)
  runtime_obf_enable_warnings(runtime_obf_basic_example)
endif()

if(RUNTIME_OBF_BUILD_TESTS)
  enable_testing()

  find_package(Python3 REQUIRED COMPONENTS Interpreter)

  add_executable(runtime_obf_tests tests/obfuscated_string_tests.cpp)
  target_link_libraries(runtime_obf_tests PRIVATE runtime_obf::runtime_obf)
  runtime_obf_enable_warnings(runtime_obf_tests)

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

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/runtime_obfConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
  ARCH_INDEPENDENT)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/runtime_obfConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/runtime_obfConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/runtime_obf")

install(
  TARGETS runtime_obf
  EXPORT runtime_obfTargets)

install(
  DIRECTORY include/
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(
  EXPORT runtime_obfTargets
  NAMESPACE runtime_obf::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/runtime_obf")

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/runtime_obfConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/runtime_obfConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/runtime_obf")
