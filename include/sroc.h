// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#pragma once

#include <stdbool.h>
#include <stdint.h>

enum sroc_error {
        ERRNOMEM = -1,
        ERRNOTFOUND = -2,
        ERRIO = -3,
};

enum sroc_data_type {
        ARRAY,
        BOOL,
        NUMBER,
        OBJECT,
        STRING,
};

// Forward declare table for use in sroc_type
struct sroc_table;

/**
 * The sroc type contains one of the valid sroc data types.
 *
 * The object type is an abstraction on top of sroc_table since they perform
 * the same function.
 */
struct sroc_type {
        enum sroc_data_type type;
        union {
                struct sroc_type **array;
                bool boolean;
                int64_t number;
                struct sroc_table *object;
                char **string;
        };
};

/**
 * A sroc item is a key value type where the key is a single word string and
 * the value is any valid sroc data type
 */
struct sroc_item {
        const char *key;
        struct sroc_type *value;
};

/**
 * A sroc array is an array of valid sroc data types
 *
 * Each item in the array must be equal in type to the rest of the items
 */
struct sroc_array {
        size_t count;
        enum sroc_data_type type;
        struct sroc_type *items;
};

/**
 * A sroc table (also refered to as a section or object) is a keyed list of
 * sroc items
 */
struct sroc_table {
        const char *key;
        size_t count;
        struct sroc_item *items;
};

struct sroc_root *sroc_init(FILE *file);
struct sroc_table *sroc_create_table(void);

int sroc_read_array(const struct sroc_root *root, const char *section,
                    const char *key, struct sroc_array **dest, size_t *length);
int sroc_read_bool(const struct sroc_root *root, const char *section,
                   const char *key, bool *dest);
int sroc_read_number(const struct sroc_root *root, const char *section,
                     const char *key, int64_t *dest);
int sroc_read_object(const struct sroc_root *root, const char *section,
                     const char *key, struct sroc_table **dest);
int sroc_read_string(const struct sroc_root *root, const char *section,
                     const char *key, char *dest);
