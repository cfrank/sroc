# All external libs

if (SROC_ENABLE_TESTING)
    # Libs required for testing
    option(WITH_EXAMPLES "Don't include cmocka examples" OFF)
    add_subdirectory(cmocka)
endif()
