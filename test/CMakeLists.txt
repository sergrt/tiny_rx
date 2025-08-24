cmake_minimum_required(VERSION 3.10)

project(tiny_rx_tests)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main gmock_main)

set(SOURCE
    complex_subscriptions.cpp
    simple_source.cpp
    sources.cpp
    stream_functions.cpp
    tiny_rx_tests.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY
  COMPILE_DEFINITIONS $<$<CONFIG:Debug>:LOG_SEVERITY=tiny_rx::utils::LogSeverity::Trace>
)

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY
  COMPILE_DEFINITIONS $<$<CONFIG:Release>:LOG_SEVERITY=tiny_rx::utils::LogSeverity::Warning>
)

enable_testing()
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${gtest_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}
)

link_directories(
    ${CMAKE_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}
    ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}
)

target_link_libraries(${PROJECT_NAME} PUBLIC tiny_rx GTest::GTest)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

#add_compile_definitions(LOG_SEVERITY1=tirx::utils::LogSeverity::${CMAKE_BUILD_TYPE})
