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
#include <sroc.h>

static void test_sroc_create_table_succeeds(void **state)
{
        char *expected_table_key = strdup("Test");
        struct sroc_table *table = sroc_create_table(expected_table_key);

        assert_string_equal(expected_table_key, table->key);
        assert_int_equal(0, table->size);
        assert_null(table->items);

        sroc_destroy_table(table);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_sroc_create_table_succeeds),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
