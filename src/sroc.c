// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_helper.h"
#include "sroc.h"
#include "string_helper.h"

struct sroc_root *sroc_parse_file(FILE *file)
{
        int64_t ftell_result;
        size_t file_size = 0;

        fseek(file, 0L, SEEK_END);
        ftell_result = ftell(file);

        if (ftell_result < 0) {
                errno = EBADF;

                return NULL;
        }

        fseek(file, 0L, SEEK_SET);
        file_size = (size_t)ftell_result;

        char *file_buffer = malloc(file_size + 1);

        if (file_buffer == NULL) {
                errno = ENOMEM;

                return NULL;
        }

        size_t bytes_read = fread(file_buffer, sizeof(char), file_size, file);

        if (bytes_read != file_size) {
                if (feof(file) != 0) {
                        // Encountered an EOF at the wrong position
                        errno = EINVAL;

                        goto free_and_return_err;
                }

                if (ferror(file) != 0) {
                        errno = EIO;

                        goto free_and_return_err;
                }
        }

        file_buffer[file_size] = '\0';

        struct sroc_root *root = sroc_parse_string(file_buffer);

        if (root == NULL) {
                goto free_and_return_err;
        }

        free(file_buffer);

        return root;

free_and_return_err:
        free(file_buffer);

        return NULL;
}

struct sroc_root *sroc_parse_string(const char *string)
{
        struct sroc_root *root = sroc_create_root();

        if (root == NULL) {
                return NULL;
        }

        struct parser_context *context = init_parser();

        if (root == NULL) {
                return NULL;
        }

        char *current_line;
        int64_t line_length;

        while ((line_length = string_get_line(string, &current_line)) >= 0) {
                context->line_num++;
                string = (string + line_length) + 1;
                free(current_line);
        }

        destroy_parser_context(context);

        return root;
}

struct sroc_root *sroc_create_root(void)
{
        struct sroc_root *root = malloc(sizeof(struct sroc_root));

        if (root == NULL) {
                errno = ENOMEM;

                return NULL;
        }

        root->items_length = 0;
        root->items = NULL;
        root->sections_length = 0;
        root->sections = NULL;

        return root;
}

struct sroc_table *sroc_create_table(char *key)
{
        struct sroc_table *table = malloc(sizeof(struct sroc_table));

        if (table == NULL) {
                return NULL;
        }

        table->key = key;
        table->size = 0;
        table->items = NULL;

        return table;
}

void sroc_destroy_root(struct sroc_root *root)
{
        for (size_t i = 0; i < root->items_length; ++i) {
                sroc_destroy_item(root->items[i]);
        }

        for (size_t i = 0; i < root->sections_length; ++i) {
                sroc_destroy_table(root->sections[i]);
        }

        free(root);
}

void sroc_destroy_array(struct sroc_array *array)
{
        for (size_t i = 0; i < array->length; ++i) {
                sroc_destroy_value(array->items[i]);
        }

        free(array);
}

void sroc_destroy_item(struct sroc_item *item)
{
        free(item->key);

        sroc_destroy_value(item->value);
}

void sroc_destroy_value(struct sroc_value *value)
{
        if (value->type == SROC_ARRAY) {
                sroc_destroy_array(value->array);
        } else if (value->type == SROC_STRING) {
                free(value->string);
        }

        free(value);
}

void sroc_destroy_table(struct sroc_table *table)
{
        free(table->key);

        for (size_t i = 0; i < table->size; ++i) {
                sroc_destroy_item(table->items[i]);
        }

        free(table);
}

