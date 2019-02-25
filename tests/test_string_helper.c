// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "../src/string_helper.h"

static void test_string_get_delimiter_not_found(void **state)
{
        int expected_result = -1;
        const char *test_string = "Nothing to be found here";
        char *dest = NULL;

        int result = string_get_delimiter(test_string, 'x', &dest);

        assert_int_equal(expected_result, result);
        assert_null(dest);
}

static void test_string_get_delimiter_found(void **state)
{
        const char *test_string = "Found me!";
        size_t expected_result = strlen(test_string) - 1; // Doesn't chomp delim
        const char *expected_string = "Found me";
        char *dest;

        int result = string_get_delimiter(test_string, '!', &dest);

        assert_int_equal(expected_result, result);
        assert_string_equal(expected_string, dest);
        // Make sure a null terminator was added
        assert_int_equal('\0', dest[result]);

        free(dest);
}

static void test_string_get_delimiter_delimiter_is_string(void **state)
{
        const char *test_string = "!";
        size_t expected_result = strlen(test_string) - 1;
        const char *expected_string = "";
        char *dest;

        int result = string_get_delimiter(test_string, '!', &dest);

        assert_int_equal(expected_result, result);
        assert_string_equal(expected_string, dest);

        free(dest);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_string_get_delimiter_not_found),
                cmocka_unit_test(test_string_get_delimiter_found),
                cmocka_unit_test(test_string_get_delimiter_delimiter_is_string),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
