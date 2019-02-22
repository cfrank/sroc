// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <cmocka.h>
#include <sroc.h>

static void test_sroc_create_root_succeeds(void **state)
{
        struct sroc_root *root = sroc_create_root();

        assert_null(root->items);
        assert_int_equal(0, root->table_count);
        assert_null(root->tables);

        sroc_destroy_root(root);
}

static void test_sroc_create_table_succeeds(void **state)
{
        const char *expected_table_key = "Test";
        struct sroc_table *table = sroc_create_table(expected_table_key);

        assert_string_equal(expected_table_key, table->key);
        assert_int_equal(0, table->count);
        assert_null(table->items);

        sroc_destroy_table(table);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(test_sroc_create_root_succeeds),
                cmocka_unit_test(test_sroc_create_table_succeeds),
        };

        return cmocka_run_group_tests(tests, NULL, NULL);
}
