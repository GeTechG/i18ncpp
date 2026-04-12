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

add_executable(i18ncpp_cache_invariant_tests
    test_cache_invariant.cpp
)

target_link_libraries(i18ncpp_cache_invariant_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_cache_invariant_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_hostile_input_tests
    test_hostile_input.cpp
)

target_link_libraries(i18ncpp_hostile_input_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_hostile_input_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_load_replace_tests
    test_load_replace.cpp
)

target_link_libraries(i18ncpp_load_replace_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_load_replace_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_format_isolation_tests
    test_format_isolation.cpp
)

target_link_libraries(i18ncpp_format_isolation_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_format_isolation_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_bool_conversion_tests
    test_bool_conversion.cpp
)

target_link_libraries(i18ncpp_bool_conversion_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_bool_conversion_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_executable(i18ncpp_key_exists_tests
    test_key_exists.cpp
)

target_link_libraries(i18ncpp_key_exists_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
target_compile_definitions(i18ncpp_key_exists_tests PRIVATE
    FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

include(GoogleTest)
gtest_discover_tests(i18ncpp_tests)
gtest_discover_tests(i18ncpp_plural_tests)
gtest_discover_tests(i18ncpp_interpolation_tests)
gtest_discover_tests(i18ncpp_formatting_tests)
gtest_discover_tests(i18ncpp_locale_mgmt_tests)
gtest_discover_tests(i18ncpp_format_cache_tests)
gtest_discover_tests(i18ncpp_cache_invariant_tests)
gtest_discover_tests(i18ncpp_hostile_input_tests)
gtest_discover_tests(i18ncpp_load_replace_tests)
gtest_discover_tests(i18ncpp_format_isolation_tests)
gtest_discover_tests(i18ncpp_bool_conversion_tests)
gtest_discover_tests(i18ncpp_key_exists_tests)

# Opt-in ThreadSanitizer regression test for the supported
# "one instance per thread" usage pattern. Disabled by default because TSan
# is unsupported on MSVC and adds instrumented build overhead on other
# toolchains. Enable on a TSan-capable toolchain with:
#   cmake -S . -B build-tsan -DI18N_ENABLE_TSAN=ON
option(I18N_ENABLE_TSAN "Build a ThreadSanitizer-instrumented test target" OFF)
if(I18N_ENABLE_TSAN AND NOT MSVC)
    add_executable(i18ncpp_thread_safety_tests
        test_thread_safety.cpp
    )
    target_link_libraries(i18ncpp_thread_safety_tests PRIVATE i18ncpp nlohmann_json GTest::gtest_main)
    target_compile_definitions(i18ncpp_thread_safety_tests PRIVATE
        FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        I18N_TSAN_BUILD
    )
    target_compile_options(i18ncpp_thread_safety_tests PRIVATE -fsanitize=thread -g)
    target_link_options(i18ncpp_thread_safety_tests PRIVATE -fsanitize=thread)
    gtest_discover_tests(i18ncpp_thread_safety_tests)
endif()
