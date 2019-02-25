# All external libs

if (SROC_ENABLE_TESTING)
    # Libs required for testing
    option(WITH_EXAMPLES "Don't include cmocka examples" OFF)
    option(ENABLE_TESTING "Don't include cmocka tests" OFF)
    add_subdirectory(cmocka)
endif()
