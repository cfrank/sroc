add_sroc_test(test-test
    SOURCES test-test.c
    LINK_LIBRARIES
        ${CMOCKA_SHARED_LIBRARY}
        sroc
    TEST_NAME TestTest
)
