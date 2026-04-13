cmake_minimum_required(VERSION 3.15)
project(i18ncpp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/json)

# Probe for C++20 <format>. Fall back to {fmt} when absent.
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_FLAGS "${CMAKE_CXX${CMAKE_CXX_STANDARD}_STANDARD_COMPILE_OPTION}")
check_cxx_source_compiles([[
#include <format>
int main() { auto s = std::format("{}", 1); return s.size() != 0 ? 0 : 0; }
]] I18N_HAS_STD_FORMAT)
unset(CMAKE_REQUIRED_FLAGS)

if(NOT I18N_HAS_STD_FORMAT)
    message(STATUS "i18ncpp: std::format unavailable — fetching {fmt} as fallback")
    include(FetchContent)
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.2.1
    )
    FetchContent_MakeAvailable(fmt)
endif()

add_library(i18ncpp src/i18ncpp.cpp)
target_include_directories(i18ncpp PUBLIC include)

target_link_libraries(i18ncpp PRIVATE nlohmann_json)
if(NOT I18N_HAS_STD_FORMAT)
    target_link_libraries(i18ncpp PRIVATE fmt::fmt)
endif()

set_target_properties(i18ncpp PROPERTIES PUBLIC_HEADER "include/i18ncpp.h")
install(TARGETS i18ncpp)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

option(BUILD_BENCHMARKS "Build benchmarks" OFF)
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
