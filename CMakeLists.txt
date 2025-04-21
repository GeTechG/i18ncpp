cmake_minimum_required(VERSION 3.15)
project(i18ncpp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(thirdparty/json)


add_library(i18ncpp src/i18ncpp.cpp)
target_include_directories(i18ncpp PUBLIC include)

target_link_libraries(i18ncpp PRIVATE nlohmann_json)

set_target_properties(i18ncpp PROPERTIES PUBLIC_HEADER "include/i18ncpp.h")
install(TARGETS i18ncpp)
