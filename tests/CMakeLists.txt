include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(i18ncpp_tests
    test_translation.cpp
)

target_link_libraries(i18ncpp_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_plural_tests
    test_plural.cpp
)

target_link_libraries(i18ncpp_plural_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_plural_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_interpolation_tests
    test_interpolation.cpp
)

target_link_libraries(i18ncpp_interpolation_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_interpolation_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_formatting_tests
    test_formatting.cpp
)

target_link_libraries(i18ncpp_formatting_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_formatting_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_locale_mgmt_tests
    test_locale_mgmt.cpp
)

target_link_libraries(i18ncpp_locale_mgmt_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_locale_mgmt_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_format_cache_tests
    test_format_cache.cpp
)

target_link_libraries(i18ncpp_format_cache_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_format_cache_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

include(GoogleTest)
gtest_discover_tests(i18ncpp_tests)
gtest_discover_tests(i18ncpp_plural_tests)
gtest_discover_tests(i18ncpp_interpolation_tests)
gtest_discover_tests(i18ncpp_formatting_tests)
gtest_discover_tests(i18ncpp_locale_mgmt_tests)
gtest_discover_tests(i18ncpp_format_cache_tests)
