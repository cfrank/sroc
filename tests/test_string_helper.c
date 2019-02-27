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

        int64_t result = string_get_delimiter(test_string, 'x', &dest);

        assert_int_equal(expected_result, result);
        assert_null(dest);
}

static void test_string_get_delimiter_found(void **state)
{
        const char *test_string = "Found me!";
        size_t expected_result = strlen(test_string) - 1; // Doesn't chomp delim
        const char *expected_string = "Found me";
        char *dest;

        int64_t result = string_get_delimiter(test_string, '!', &dest);

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

        int64_t result = string_get_delimiter(test_string, '!', &dest);

        assert_int_equal(expected_result, result);
        assert_string_equal(expected_string, dest);

        free(dest);
}

static void test_string_get_line_found(void **state)
{
        const char *test_string = "My First String\nMy Second String\n";
        const char *expected_first_string = "My First String";
        const char *expected_second_string = "My Second String";
        size_t expected_result = strlen(expected_first_string);
        char *dest;

        int64_t result = string_get_line(test_string, &dest);

        assert_int_equal(expected_result, result);
        assert_string_equal(expected_first_string, dest);

        free(dest);

        string_get_line((test_string + result + 1), &dest);

        assert_string_equal(expected_second_string, dest);

        free(dest);
}

static void test_string_find_first_nonspace_empty(void **state)
{
        int64_t expected_result = -1;
        const char *test_string = "";

        int64_t result = string_find_first_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_first_nonspace_no_nonspace(void **state)
{
        int64_t expected_result = -1;
        const char *test_string = "     ";

        int64_t result = string_find_first_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_first_nonspace_single_char(void **state)
{
        int64_t expected_result = 0;
        const char *test_string = "C";

        int64_t result = string_find_first_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_first_nonspace_tab(void **state)
{
        int64_t expected_result = 1;
        const char *test_string = "\tHi";

        int64_t result = string_find_first_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_first_nonspace_large_space(void **state)
{
        int64_t expected_result = 10;
        const char *test_string = "          Hi";

        int64_t result = string_find_first_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_first_nonspace_surrounded_sentence(void **state)
{
        int64_t expected_result = 5;
        const char *test_string = "     Hello world! - Test     ";

        int64_t result = string_find_first_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_string_get_delimiter_not_found),
                cmocka_unit_test(test_string_get_delimiter_found),
                cmocka_unit_test(test_string_get_delimiter_delimiter_is_string),
                cmocka_unit_test(test_string_get_line_found),
                cmocka_unit_test(test_string_find_first_nonspace_empty),
                cmocka_unit_test(test_string_find_first_nonspace_no_nonspace),
                cmocka_unit_test(test_string_find_first_nonspace_single_char),
                cmocka_unit_test(test_string_find_first_nonspace_tab),
                cmocka_unit_test(test_string_find_first_nonspace_large_space),
                cmocka_unit_test(
                        test_string_find_first_nonspace_surrounded_sentence),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
