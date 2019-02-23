// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum sroc_error {
        ERRNOKEY = -1,
        ERRNOSECTION = -2,
        ERRNOMEM = -3,
        ERRIO = -4,
};

enum sroc_type {
        ARRAY,
        BOOL,
        NUMBER,
        STRING,
};

// Forward declare sroc_type for use with parent types
struct sroc_value;

/**
 * A sroc array is an array of valid sroc value
 *
 * Each item in the array must be equal in type to the rest of the items
 */
struct sroc_array {
        size_t length;
        enum sroc_data_type type;
        struct sroc_value *items;
};

/**
 * The sroc value contains one of the valid sroc values
 */
struct sroc_value {
        enum sroc_type type;
        union {
                struct sroc_array *array;
                bool boolean;
                int64_t number;
                char *string;
        };
};

/**
 * A sroc item is a key value type where the key is a single word string and
 * the value is any valid sroc value
 */
struct sroc_item {
        const char *key;
        struct sroc_value *value;
};

/**
 * The sroc table is the basis for both root files and sections. It contains a
 * key (which can be NULL when no sections exist in the root) and a list of
 * sroc items
 *
 * The size is the number of items in that particular section
 */
struct sroc_table {
        // If NULL then this is the root
        const char *key;
        size_t size;
        struct sroc_item *items;
};

struct sroc_table *sroc_init(FILE *file);
struct sroc_table *sroc_create_table(const char *key);

// Get a single section from the root table
int sroc_get_section(const struct sroc_table *root, const char *section,
                     struct sroc_table **dest);
int sroc_read_array(const struct sroc_table *root, const char *section,
                    const char *key, struct sroc_array **dest, size_t *length);
int sroc_read_bool(const struct sroc_table *root, const char *section,
                   const char *key, bool *dest);
int sroc_read_number(const struct sroc_table *root, const char *section,
                     const char *key, int64_t *dest);
int sroc_read_string(const struct sroc_table *root, const char *section,
                     const char *key, char **dest);

void sroc_destroy_array(struct sroc_array *array);
void sroc_destroy_type(struct sroc_type *type);
void sroc_destroy_table(struct sroc_table *table);
