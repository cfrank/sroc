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
        int64_t result = string_get_delimiter(test_string, &dest, 'x', false);

        assert_int_equal(expected_result, result);
        assert_null(dest);
}

static void test_string_get_delimiter_found(void **state)
{
        const char *test_string = "Found me!";
        size_t expected_result = strlen(test_string) - 1; // Doesn't chomp delim
        const char *expected_string = "Found me";
        char *dest;
        int64_t result = string_get_delimiter(test_string, &dest, '!', false);

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
        int64_t result = string_get_delimiter(test_string, &dest, '!', false);

        assert_int_equal(expected_result, result);
        assert_string_equal(expected_string, dest);

        free(dest);
}

static void test_string_get_delimiter_consume(void **state)
{
        const char *test_string = "Hello! Goodbye";
        const char *expected_string = "Hello!";
        size_t expected_result = strlen(expected_string);
        char *dest;
        int64_t result = string_get_delimiter(test_string, &dest, '!', true);

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

static void test_string_find_first_nonspace_null(void **state)
{
        int64_t expected_result = -1;
        int64_t result = string_find_first_nonspace(NULL);

        assert_int_equal(expected_result, result);
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

static void test_string_find_last_nonspace_null(void **state)
{
        int64_t expected_result = -1;
        int64_t result = string_find_last_nonspace(NULL);

        assert_int_equal(expected_result, result);
}

static void test_string_find_last_nonspace_empty(void **state)
{
        int64_t expected_result = -1;
        const char *test_string = "";
        int64_t result = string_find_last_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_last_nonspace_no_nonspace(void **state)
{
        int64_t expected_result = -1;
        const char *test_string = "          ";
        int64_t result = string_find_last_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_last_nonspace_single_char(void **state)
{
        int64_t expected_result = 0;
        const char *test_string = "C";
        int64_t result = string_find_last_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_last_nonspace_tab(void **state)
{
        int64_t expected_result = 1;
        const char *test_string = "Hi\t";
        int64_t result = string_find_last_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_last_nonspace_large_space(void **state)
{
        int64_t expected_result = 4;
        const char *test_string = "Hello          ";
        int64_t result = string_find_last_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_find_last_nonspace_surrounded_sentence(void **state)
{
        int64_t expected_result = 23;
        const char *test_string = "     Hello world! - Test     ";
        int64_t result = string_find_last_nonspace(test_string);

        assert_int_equal(expected_result, result);
}

static void test_string_splice_null(void **state)
{
        int64_t expected_result = -1;
        char *dest;
        int64_t result = string_splice(NULL, &dest, 0, 0);

        assert_int_equal(expected_result, result);
}

static void test_string_splice_negative_start(void **state)
{
        int64_t expected_result = -1;
        const char *test_string = "Test";
        char *dest;
        int64_t result = string_splice(test_string, &dest, -1, 0);

        assert_int_equal(expected_result, result);
}

static void test_string_splice_larger_start(void **state)
{
        int64_t expected_result = -1;
        const char *test_string = "Test";
        char *dest;
        int64_t result = string_splice(test_string, &dest, 10, 0);

        assert_int_equal(expected_result, result);
}

static void test_string_splice_large_end(void **state)
{
        int64_t expected_result = -1;
        const char *test_string = "Test";
        char *dest;
        int64_t result = string_splice(test_string, &dest, 0, 5);

        assert_int_equal(expected_result, result);
}

static void test_string_splice_equal_indexes(void **state)
{
        int64_t expected_result = 0;
        const char *test_string = "Test";
        const char *expected_string = "";
        char *dest;
        int64_t result = string_splice(test_string, &dest, 1, 1);

        assert_int_equal(expected_result, result);
        assert_string_equal(expected_string, dest);
        assert_int_equal('\0', dest[result]);

        free(dest);
}

static void test_string_splice_bounds(void **state)
{
        const char *test_string = "Hello world!";
        char *dest;
        int64_t result = string_splice(
                test_string, &dest, 0, (int64_t)strlen(test_string));

        assert_int_equal(strlen(test_string), result);
        assert_string_equal(test_string, dest);

        free(dest);
}

static void test_string_splice_valid(void **state)
{
        const char *test_string = "Hello world!";
        const char *expected_string = "lo wo";
        char *dest;
        int64_t result = string_splice(test_string, &dest, 3, 8);

        assert_int_equal(strlen(expected_string), result);
        assert_string_equal(expected_string, dest);

        free(dest);
}

// Most of the tests for the following function have already been accomplished
// in above tests since string_strip_surrounding_space just calls each of the
// above functions to perform the logic

static void test_string_strip_surrounding_space_null(void **state)
{
        int64_t expected_result = -1;
        char *dest;
        int64_t result = string_strip_surrounding_space(NULL, &dest);

        assert_int_equal(expected_result, result);
}

static void test_string_strip_surrounding_space_null_dest(void **state)
{
        int64_t expected_result = -1;
        const char *test_string = "Hello world";
        int64_t result = string_strip_surrounding_space(test_string, NULL);

        assert_int_equal(expected_result, result);
}

static void test_string_strip_surrounding_space_single(void **state)
{
        const char *expected_string = "f";
        const char *test_string = "  f  ";
        char *dest;
        int64_t result = string_strip_surrounding_space(test_string, &dest);

        assert_int_equal(result, strlen(expected_string));
        assert_string_equal(expected_string, dest);
        assert_int_equal('\0', dest[strlen(expected_string)]);

        free(dest);
}

static void test_string_strip_surrounding_space_sentence(void **state)
{
        const char *expected_string = "Hello world!";
        const char *test_string = "    Hello world!    ";
        char *dest;
        int64_t result = string_strip_surrounding_space(test_string, &dest);

        assert_int_equal(result, strlen(expected_string));
        assert_string_equal(expected_string, dest);

        free(dest);
}

static void test_string_strip_surrounding_space_single_space(void **state)
{
        const char *expected_string = "h";
        const char *test_string = "h ";
        char *dest;
        int64_t result = string_strip_surrounding_space(test_string, &dest);

        assert_int_equal(result, strlen(expected_string));
        assert_string_equal(expected_string, dest);

        free(dest);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_string_get_delimiter_not_found),
                cmocka_unit_test(test_string_get_delimiter_found),
                cmocka_unit_test(test_string_get_delimiter_delimiter_is_string),
                cmocka_unit_test(test_string_get_delimiter_consume),
                cmocka_unit_test(test_string_get_line_found),
                cmocka_unit_test(test_string_find_first_nonspace_null),
                cmocka_unit_test(test_string_find_first_nonspace_empty),
                cmocka_unit_test(test_string_find_first_nonspace_no_nonspace),
                cmocka_unit_test(test_string_find_first_nonspace_single_char),
                cmocka_unit_test(test_string_find_first_nonspace_tab),
                cmocka_unit_test(test_string_find_first_nonspace_large_space),
                cmocka_unit_test(
                        test_string_find_first_nonspace_surrounded_sentence),
                cmocka_unit_test(test_string_find_last_nonspace_null),
                cmocka_unit_test(test_string_find_last_nonspace_empty),
                cmocka_unit_test(test_string_find_last_nonspace_no_nonspace),
                cmocka_unit_test(test_string_find_last_nonspace_single_char),
                cmocka_unit_test(test_string_find_last_nonspace_tab),
                cmocka_unit_test(test_string_find_last_nonspace_large_space),
                cmocka_unit_test(
                        test_string_find_last_nonspace_surrounded_sentence),
                cmocka_unit_test(test_string_splice_null),
                cmocka_unit_test(test_string_splice_negative_start),
                cmocka_unit_test(test_string_splice_larger_start),
                cmocka_unit_test(test_string_splice_large_end),
                cmocka_unit_test(test_string_splice_equal_indexes),
                cmocka_unit_test(test_string_splice_bounds),
                cmocka_unit_test(test_string_splice_valid),
                cmocka_unit_test(test_string_strip_surrounding_space_null),
                cmocka_unit_test(test_string_strip_surrounding_space_null_dest),
                cmocka_unit_test(test_string_strip_surrounding_space_single),
                cmocka_unit_test(test_string_strip_surrounding_space_sentence),
                cmocka_unit_test(
                        test_string_strip_surrounding_space_single_space),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
