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

static int64_t get_file_size(FILE *file)
{
        fseek(file, 0L, SEEK_END);
        int64_t size = ftell(file);

        if (size < 0) {
                errno = EBADF;

                return -1;
        }

        fseek(file, 0L, SEEK_SET);

        return size;
}

/**
 * Takes an uninitalized string and places the contents of a file into it
 *
 * The buffer is initialized with file_size + 1 bytes and must be freed by the
 * caller
 *
 * file_size bytes are read from file and placed in the buffer. The buffer is
 * then appended with a null terminator
 */
static int read_file_into_buffer(FILE *file, char **buffer, size_t file_size)
{
        *buffer = malloc(file_size + 1);

        if (*buffer == NULL) {
                errno = ENOMEM;

                return -1;
        }

        size_t bytes_read = fread(*buffer, 1, file_size, file);

        if (bytes_read != file_size) {
                if (feof(file) != 0) {
                        // Encountered an EOF at the wrong pos
                        errno = EINVAL;

                        goto free_and_err;
                }

                if (ferror(file) != 0) {
                        errno = EIO;

                        goto free_and_err;
                }
        }

        (*buffer)[file_size] = '\0';

        return 0;

free_and_err:
        free(*buffer);

        return -1;
}

/**
 * Update the parsing context after a single character iteration through the
 * parser buffer.
 *
 * It cannot be assumed that the previous value will always be n-1 since the
 * context could have been updated inside of the parse loop. So the only job
 * of this function is to increment by 1
 */
static void increment_parser_context(struct parser_context *context)
{
        if (context->buffer[context->pos] == '\n') {
                // Handle a new line in the buffer
                ++context->line_num;
                context->col_num = 0;
        }

        ++context->pos;
}

/**
 * Consume the currently focused line refrenced to by the context
 *
 * We want to land on the newline itself since the parse loop will continue to
 * incremement one more time
 */
static void consume_line(struct parser_context *context)
{
        char ch;

        // TODO: (cf) Maybe just do a pos search for \n
        while ((ch = context->buffer[context->pos]) != '\n') {
                increment_parser_context(context);
        }
}

/**
 * Get a section and add it to the context
 */
static bool add_section(struct parser_context *context)
{
        struct sroc_table *section = get_section(context);

        if (section == NULL) {
                return false;
        }

        printf("FOUND SECTION: %s\n", section->key);

        sroc_destroy_table(section);

        return true;
}

struct sroc_root *sroc_parse_file(FILE *file)
{
        int64_t ftell_result = get_file_size(file);

        if (ftell_result < 0) {
                return NULL;
        }

        size_t file_size = (size_t)ftell_result;

        char *file_buffer;

        if (read_file_into_buffer(file, &file_buffer, file_size) != 0) {
                return NULL;
        }

        struct sroc_root *root = sroc_parse_string(file_buffer);

        if (root == NULL) {
                free(file_buffer);

                return NULL;
        }

        free(file_buffer);

        return root;
}

struct sroc_root *sroc_parse_string(const char *string)
{
        struct sroc_root *root = sroc_create_root();

        if (root == NULL) {
                return NULL;
        }

        struct parser_context *context = init_parser();

        if (context == NULL) {
                return NULL;
        }

        context->buffer = string;

        char cur_ch;

        /**
         * This while loop should not loop over every single char it should
         * hit the first char of each line. The logic inside the loop should
         * handle each line and update the pos to the last char in the line
         * after which incremement_parser_context will incremement the pos to
         * the beginning of the next line
         */
        while ((cur_ch = context->buffer[context->pos]) != '\0') {
                switch (char_to_token(cur_ch)) {
                case OPEN_BRACKET:
                        // Handle a new section
                        if (!add_section(context)) {
                                goto free_and_err;
                        }

                        break;
                case ALPHA_CHAR:
                        // Handle a new declaration
                        if (!is_valid_declaration(context)) {
                                goto free_and_err;
                        }
                        break;
                case COMMENT_START:
                        // Skip the line since it's a comment
                        consume_line(context);
                        break;
                default:
                        // Invalid token found
                        break;
                }

                increment_parser_context(context);
        }

        destroy_parser_context(context);

        return root;

free_and_err:
        sroc_destroy_root(root);
        destroy_parser_context(context);

        return NULL;
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
                sroc_destroy_item(&root->items[i]);
        }

        for (size_t i = 0; i < root->sections_length; ++i) {
                sroc_destroy_table(&root->sections[i]);
        }

        free(root);
}

void sroc_destroy_array(struct sroc_array *array)
{
        for (size_t i = 0; i < array->length; ++i) {
                sroc_destroy_value(&array->items[i]);
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
                sroc_destroy_item(&table->items[i]);
        }

        free(table);
}

