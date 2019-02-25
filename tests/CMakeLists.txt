add_sroc_test(test-creation
    SOURCES test_creation.c
    LINK_LIBRARIES
        ${CMOCKA_SHARED_LIBRARY}
        sroc
    TEST_NAME TestCreation
)

add_sroc_test(test-string-helper
    SOURCES test_string_helper.c
    LINK_LIBRARIES
        ${CMOCKA_SHARED_LIBRARY}
        sroc
    TEST_NAME TestStringHelper
)
