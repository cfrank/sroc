// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <cmocka.h>
#include "util.c"

static void test_read_until_delimiter_null_buffer(void **state)
{
        ssize_t expected_result = -1;
        ssize_t result = read_until_delimiter(NULL, 'i', NULL, NULL, false);

        assert_int_equal(expected_result, result);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_read_until_delimiter_null_buffer),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
