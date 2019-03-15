// Copyright 2019 Chris Frank
// Licensed under BSD-3-Clause
// Refer to the license.txt file included in the root of the project

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "parse_helper.h"
#include "sroc.h"
#include "string_helper.h"

/**
 * Get a section and add it to the context
 */
static bool add_section(struct parser_context *context)
{
        struct sroc_table *section = get_section(context);

        if (section == NULL) {
                return false;
        }

        sroc_destroy_table(section);

        return true;
}

/**
 * Consume the currently focused line refrenced to by the context
 *
 * This will place the pos at the end of the line
 */
static void consume_line(struct parser_context *context)
{
        while (context->buffer[context->pos] != '\n') {
                increment_parser_context(context);
        }
}

/**
 * Check if a value is spread out over multiple lines
 *
 * If a non-terminated value is found, attempt to set the value as the new
 * multi-line value along with setting the length to the new length of the
 * value. Otherwise do nothing with both the value and length.
 */
static int check_for_multiline_value(const struct parser_context *context,
                                     char **value, int64_t *length)
{
        enum token_type start_token = char_to_token(*value[0]);
        enum token_type close_token = char_to_token(*(value)[*length - 1]);
        char closing_char;

        if (start_token != OPEN_BRACKET && start_token != QUOTE) {
                // New lines are only valid in strings and arrays
                return 0;
        }

        closing_char = (start_token == OPEN_BRACKET) ? ']' : '"';

        if (close_token == char_to_token(closing_char)) {
                // Not a multiline string
                return 0;
        }

        char *multiline_value;
        int64_t multiline_length
                = string_get_delimiter(context->buffer + context->pos,
                                       &multiline_value,
                                       closing_char,
                                       true);

        if (multiline_length < 0) {
                return -1;
        }

        // Found a valid multiline value - free old value and set to new
        free(*value);

        *value = multiline_value;
        *length = multiline_length;

        return 0;
}

/**
 * Get the key from a argument pointing to the beginning of a declaration
 *
 * If a key is returned from this function the context will be updated to point
 * to the first char after the EQUAL token
 */
static char *get_key_from_declaration(struct parser_context *context)
{
        char *key;
        char *stripped_key;
        int64_t key_len = string_get_delimiter(
                context->buffer + context->pos, &key, '=', false);

        if (key_len < 0) {
                return NULL;
        }

        if (string_strip_surrounding_space(key, &stripped_key) < 0) {
                free(key);

                return NULL;
        }

        free(key);

        // Set the parser pos to the '='
        set_parser_context_pos(context, context->pos + ((size_t)key_len + 1));

        return stripped_key;
}

/**
 * Get the value from a argument pointing to the '=' inside a declaration
 *
 * If the value is found and returned from this function the context will be
 * updated to point to the end of the declaration
 */
static char *get_value_from_declaration(struct parser_context *context)
{
        int64_t value_start_pos
                = string_find_first_nonspace(context->buffer + context->pos);

        if (value_start_pos < 0) {
                return NULL;
        }

        set_parser_context_pos(context, context->pos + (size_t)value_start_pos);

        char *value;
        int64_t value_length;

        value_length = string_get_line(context->buffer + context->pos, &value);

        // Check if we need to read multiple lines
        check_for_multiline_value(context, &value, &value_length);

        printf("%s\n", value);

        return value;
}

enum token_type char_to_token(char input)
{
        switch (input) {
        case ']':
                return CLOSE_BRACKET;
        case ';':
        case '#':
                return COMMENT_START;
        case ',':
                return COMMA;
        case '=':
                return EQUAL;
        case '\\':
                return ESCAPE;
        case '-':
                return NEGATIVE;
        case '[':
                return OPEN_BRACKET;
        case '.':
                return PERIOD;
        case '"':
                return QUOTE;
        default:
                break;
        }

        if (isalpha(input)) {
                return ALPHA_CHAR;
        }

        if (isdigit(input)) {
                return NUMERIC_CHAR;
        }

        return UNKNOWN;
}

int parse_line(struct parser_context *context, char ch)
{
        switch (char_to_token(ch)) {
        case OPEN_BRACKET:
                // Handle a new section
                if (!add_section(context)) {
                        return -1;
                }

                return 0;
        case ALPHA_CHAR:
                if (get_declaration(context) == NULL) {
                        return -1;
                }

                return 0;
        case COMMENT_START:
                // Skip the line since it's a comment
                consume_line(context);

                return 0;
        default:
                return -1;
        }

        return -1;
}

struct parser_context *init_parser(void)
{
        struct parser_context *context = malloc(sizeof(struct parser_context));

        if (context == NULL) {
                errno = ENOMEM;

                return NULL;
        }

        context->buffer = NULL;
        context->pos = 0;
        context->line_num = 0;
        context->col_num = 0;
        context->current_value = NULL;
        context->current_table = NULL;

        return context;
}

/**
 * Update the parsing context after a single character iteration through the
 * parser buffer.
 *
 * When a new line is encountered reset the col_num and increment the line_num
 */
void increment_parser_context(struct parser_context *context)
{
        if (context->buffer[context->pos] == '\n') {
                // Handle a new line in the buffer
                ++context->line_num;
                context->col_num = 0;
        }

        ++context->pos;
}

/**
 * Move the parsing context forward num bytes - keeping track of line_num and
 * col_num
 */
void set_parser_context_pos(struct parser_context *context, size_t index)
{
        if (index < context->pos) {
                return;
        }

        while (context->buffer[context->pos] != '\0' && context->pos != index) {
                increment_parser_context(context);
        }
}

struct sroc_value *get_declaration(struct parser_context *context)
{
        char *key = get_key_from_declaration(context);

        if (key == NULL) {
                return NULL;
        }

        printf("The key is |%s|\n", key);

        char *value = get_value_from_declaration(context);

        printf("The Value is |%s|\n", value);

        free(key);
        free(value);

        return NULL;
}

struct sroc_table *get_section(struct parser_context *context)
{
        if (char_to_token(context->buffer[context->pos]) != OPEN_BRACKET) {
                return NULL;
        }

        char *key;

        int64_t key_len = string_get_delimiter(
                context->buffer + (context->pos + 1), &key, ']', false);

        if (key_len < 0) {
                return NULL;
        }

        struct sroc_table *section = sroc_create_table(key);

        if (section == NULL) {
                free(key);

                return NULL;
        }

        set_parser_context_pos(context, context->pos + ((size_t)key_len + 2));

        return section;
}

void destroy_parser_context(struct parser_context *context)
{
        free(context);
}
