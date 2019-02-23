add_sroc_test(test-creation
    SOURCES test-creation.c
    LINK_LIBRARIES
        ${CMOCKA_SHARED_LIBRARY}
        sroc
    TEST_NAME TestCreation
)

add_sroc_test(test-string-util
    SOURCES test-string-util.c
    LINK_LIBRARIES
        ${CMOCKA_SHARED_LIBRARY}
        sroc
    TEST_NAME TestStringUtil
)
