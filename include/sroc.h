// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum sroc_error {
        SROC_ERRNOKEY = -1,
        SROC_ERRNOSECTION = -2,
        SROC_ERRNOMEM = -3,
        SROC_ERRIO = -4,
};

enum sroc_type {
        SROC_ARRAY,
        SROC_BOOL,
        SROC_NUMBER,
        SROC_STRING,
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
        enum sroc_type type;
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
        char *key;
        struct sroc_value *value;
};

/**
 * A sroc table (or section) is a keyed list of sroc items
 */
struct sroc_table {
        char *key;
        size_t size;
        struct sroc_item *items;
};

/**
 * The sroc root is the root of the file. It contains all the sroc items that
 * are not under a section as well as all the sections in the configuration
 * file
 */
struct sroc_root {
        size_t items_length;
        struct sroc_item *items;
        size_t sections_length;
        struct sroc_table *sections;
};

struct sroc_root *sroc_parse_file(FILE *file);
struct sroc_root *sroc_parse_string(const char *buffer);

struct sroc_root *sroc_create_root(void);
struct sroc_table *sroc_create_table(char *key);

// Get a single section from the root table
int sroc_get_section(const struct sroc_table *root, const char *section,
                     struct sroc_table *dest);
int sroc_read_array(const struct sroc_table *root, const char *section,
                    const char *key, struct sroc_array *dest, size_t *length);
int sroc_read_bool(const struct sroc_table *root, const char *section,
                   const char *key, bool *dest);
int sroc_read_number(const struct sroc_table *root, const char *section,
                     const char *key, int64_t *dest);
int sroc_read_string(const struct sroc_table *root, const char *section,
                     const char *key, char **dest);

void sroc_destroy_root(struct sroc_root *root);
void sroc_destroy_array(struct sroc_array *array);
void sroc_destroy_item(struct sroc_item *item);
void sroc_destroy_value(struct sroc_value *value);
void sroc_destroy_table(struct sroc_table *table);
