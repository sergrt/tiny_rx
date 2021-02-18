cmake_minimum_required(VERSION 3.10)

project(tirx_tests)

set(SOURCE
    SimpleSource.cpp
    StreamFunctions.cpp
    TirxTests.cpp)

add_executable(tirx_tests ${SOURCE})


set_property(TARGET tirx_tests APPEND PROPERTY
  COMPILE_DEFINITIONS $<$<CONFIG:Debug>:LOG_SEVERITY=tirx::utils::LogSeverity::Trace>
)

set_property(TARGET tirx_tests APPEND PROPERTY
  COMPILE_DEFINITIONS $<$<CONFIG:Release>:LOG_SEVERITY=tirx::utils::LogSeverity::Warning>
)

add_subdirectory(gtest)
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

target_link_libraries(tirx_tests tirx gtest)

set_property(TARGET tirx_tests PROPERTY CXX_STANDARD 17)

#add_compile_definitions(LOG_SEVERITY1=tirx::utils::LogSeverity::${CMAKE_BUILD_TYPE})

