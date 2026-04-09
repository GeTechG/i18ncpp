add_executable(bench_lookup bench_lookup.cpp)
target_include_directories(bench_lookup PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bench_lookup PRIVATE i18ncpp nlohmann_json)
target_compile_definitions(bench_lookup PRIVATE
    FIXTURES_DIR="${CMAKE_SOURCE_DIR}/tests/fixtures"
)

add_executable(bench_interpolation bench_interpolation.cpp)
target_include_directories(bench_interpolation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bench_interpolation PRIVATE i18ncpp nlohmann_json)
target_compile_definitions(bench_interpolation PRIVATE
    FIXTURES_DIR="${CMAKE_SOURCE_DIR}/tests/fixtures"
)

add_executable(bench_formatting bench_formatting.cpp)
target_include_directories(bench_formatting PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bench_formatting PRIVATE i18ncpp nlohmann_json)
target_compile_definitions(bench_formatting PRIVATE
    FIXTURES_DIR="${CMAKE_SOURCE_DIR}/tests/fixtures"
)
